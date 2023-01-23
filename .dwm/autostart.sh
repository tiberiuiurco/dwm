dwmblocks &
nitrogen --restore &
nm-applet &
volumeicon &
# picom &
# xrandr --output HDMI-1 --left-of eDP-1 --mode 1920x1080
xrandr --output DP-1 --right-of DP-4 --mode 1920x1080
# fcitx -d &
ibus-daemon -drx
/snap/bin/emacs --daemon &# Start Emacs Server
