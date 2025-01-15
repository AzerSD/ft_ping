#!/bin/bash

sudo tcpdump -i wlp6s0 host 192.168.0.142 and 8.8.8.8 -vvv -x

