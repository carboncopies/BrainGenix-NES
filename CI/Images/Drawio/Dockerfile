FROM rlespinasse/drawio-desktop-headless:latest

ARG REPO_ROOT_DIR
# ENV DRAWIO_VERSION 21.5.1

# RUN set -e -u -x; \
#     apt update; apt -y upgrade; \
#     DEBIAN_FRONTEND=noninteractive apt -y install \
#         curl xvfb nano libgtk-3-0 libnotify4 libnss3 libxss1 libasound2 libxtst6 xdg-utils libatspi2.0-0 libappindicator3-1 libsecret-1-0 libgbm1; \
#     curl -LJO "https://github.com/jgraph/drawio-desktop/releases/download/v${DRAWIO_VERSION}/drawio-x86_64-${DRAWIO_VERSION}.AppImage"; \
#     chmod a+x "drawio-x86_64-${DRAWIO_VERSION}.AppImage"; \
#     "./drawio-x86_64-${DRAWIO_VERSION}.AppImage" --appimage-extract; \
#     rm "drawio-x86_64-${DRAWIO_VERSION}.AppImage"

# COPY /CI/Images/Drawio/drawio /usr/local/bin/
# RUN chmod +x /usr/local/bin/drawio