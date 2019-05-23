#!/bin/sh
INCLUDE_ONLY=1

. ../netifd-proto.sh
. ./ppp.sh
init_proto "$@"

proto_3g_init_config() {
	no_device=1
	available=1
	ppp_generic_init_config
	proto_config_add_string "device:device"
	proto_config_add_string "apn"
	proto_config_add_string "service"
	proto_config_add_string "pincode"
	proto_config_add_string "dialnumber"
}

proto_3g_setup() {
	local interface="$1"
	local chat

	json_get_var device device
	json_get_var apn apn
	json_get_var service service
	json_get_var pincode pincode
	json_get_var dialnumber dialnumber

	[ "$device" = "auto" ] && {
		device=""
		USBDEV=`lsusb | awk -F 'ID ' '{print $2}' | tr '[a-z]' '[A-Z]'`
		for usb in ${USBDEV}; do
			case "${usb}" in
				"05C6:9025")
					logger "Found USB/LTE model, VID:PID is '${usb}'"
					device="/dev/ttyUSB3"
					break
				;;

				"19D2:1476")
					logger "Found ZTE-ME3630 USB/LTE model, VID:PID is '${usb}'"
					device="/dev/ttyUSB2"
					break
				;;
				
				"12D1:1001")
					logger "Found HUAWEI-EM730 USB/LTE model, VID:PID is '${usb}'"
					device="/dev/ttyUSB0"
					break
				;;

				"161C:9115")
					logger "Found QUALCOMM-MDM9600 USB/LTE model, VID:PID is '${usb}'"
					device="/dev/ttyUSB1"
					break
				;;
				
				"05c6:9005")
					logger "Found FM926 USB/LTE model, VID:PID is '${usb}'"
					device="/dev/ttyUSB1"
				;;
			esac
		done
	}

	[ -e "$device" ] || {
		proto_set_available "$interface" 0
		return 1
	}

	case "$service" in
		lte_cmcc)
			# China Mobile
			chat="/etc/chatscripts/myscripts/cmcc-connect-chat"
			[ -z "$dialnumber" ] && dialnumber="*99***1#"
		;;
		lte_cucc)
			# China Unicom
			chat="/etc/chatscripts/myscripts/ChinaUnicom.chat"
			[ -z "$dialnumber" ] && dialnumber="*99#"
		;;
		lte_ctcc)
			# China Telecom
			chat="/etc/chatscripts/myscripts/lte-connect-chat"
			[ -z "$dialnumber" ] && dialnumber="*#777"
		;;
		cdma|evdo)
			chat="/etc/chatscripts/evdo.chat"
		;;
		*)
			chat="/etc/chatscripts/3g.chat"
			cardinfo=$(gcom -d "$device" -s /etc/gcom/getcardinfo.gcom)
			if echo "$cardinfo" | grep -q Novatel; then
				case "$service" in
					umts_only) CODE=2;;
					gprs_only) CODE=1;;
					*) CODE=0;;
				esac
				export MODE="AT\$NWRAT=${CODE},2"
			elif echo "$cardinfo" | grep -q Option; then
				case "$service" in
					umts_only) CODE=1;;
					gprs_only) CODE=0;;
					*) CODE=3;;
				esac
				export MODE="AT_OPSYS=${CODE}"
			elif echo "$cardinfo" | grep -q "Sierra Wireless"; then
				SIERRA=1
			elif echo "$cardinfo" | grep -qi huawei; then
				case "$service" in
					umts_only) CODE="14,2";;
					gprs_only) CODE="13,1";;
					*) CODE="2,2";;
				esac
				export MODE="AT^SYSCFG=${CODE},3FFFFFFF,2,4"
			fi

			if [ -n "$pincode" ]; then
				PINCODE="$pincode" gcom -d "$device" -s /etc/gcom/setpin.gcom || {
					proto_notify_error "$interface" PIN_FAILED
					proto_block_restart "$interface"
					return 1
				}
			fi
			[ -n "$MODE" ] && gcom -d "$device" -s /etc/gcom/setmode.gcom

			# wait for carrier to avoid firmware stability bugs
			[ -n "$SIERRA" ] && {
				gcom -d "$device" -s /etc/gcom/getcarrier.gcom || return 1
			}

			if [ -z "$dialnumber" ]; then
				dialnumber="*99***1#"
			fi

		;;
	esac

	connect="${apn:+USE_APN=$apn }DIALNUMBER=$dialnumber /usr/sbin/chat -t5 -v -E -f $chat"
	ppp_generic_setup "$interface" \
		noaccomp \
		nopcomp \
		novj \
		nobsdcomp \
		noauth \
		lock \
		crtscts \
		115200 "$device"
	return 0
}

proto_3g_teardown() {
	proto_kill_command "$interface"
}

add_protocol 3g
