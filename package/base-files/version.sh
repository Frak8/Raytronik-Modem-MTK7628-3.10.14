#!/bin/bash
git rev-parse --short=5 HEAD > /home/amir/Project/Modem/Raytronik-Modem-MTK7628-3.10.14/package/base-files/files/etc/git_date_version
date '+%D -- %R'>> /home/amir/Project/Modem/Raytronik-Modem-MTK7628-3.10.14/package/base-files/files/etc/git_date_version
