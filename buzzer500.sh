#!/bin/sh

if [ ! -d /sys/class/pwm/pwmchip1/ ]; then
	echo 'PWM not found! (/sys/class/pwm/pwmchip1/)'
	exit 1
fi

if [ ! -d /sys/class/pwm/pwmchip1/pwm0 ]; then
	echo 0 > /sys/class/pwm/pwmchip1/export
fi

if [ ! -d /sys/class/pwm/pwmchip1/pwm0 ]; then
        echo 'PWM export failed!'
	exit 2
fi

cd /sys/class/pwm/pwmchip1/pwm0
echo 1000000 > duty_cycle
echo 2000000 > period
echo 1000000 > duty_cycle
echo 2000000 > period
echo 1 > enable
sleep 0.125
echo 0 > enable

