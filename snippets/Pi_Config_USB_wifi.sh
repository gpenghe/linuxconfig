# in /etc/wpa_supplicant/wpa_supplicant.conf:
# add:
network={
    ssid="testing"
    key_mgmt=NONE
}

# then optionally, do:
# sudo wpa-cli reconfigure
