#!/bin/sh
set -e -x

cat << EOF > configure/RELEASE.local
EPICS_BASE=$HOME/.source/epics-base
EOF

install -d "$HOME/.source"
cd "$HOME/.source"

git clone --quiet --depth 5 --branch "$BRBASE" https://github.com/${REPOBASE:-epics-base}/epics-base.git epics-base
(cd epics-base && git log -n1 )

EPICS_HOST_ARCH=`sh epics-base/startup/EpicsHostArch`

case "$CMPLR" in
clang)
  echo "Host compiler is clang"
  cat << EOF >> epics-base/configure/os/CONFIG_SITE.Common.$EPICS_HOST_ARCH
GNU         = NO
CMPLR_CLASS = clang
CC          = clang
CCC         = clang++
EOF

  # hack
  sed -i -e 's/CMPLR_CLASS = gcc/CMPLR_CLASS = clang/' epics-base/configure/CONFIG.gnuCommon

  clang --version
  ;;
*)
  echo "Host compiler is default"
  gcc --version
  ;;
esac

cat <<EOF >> epics-base/configure/CONFIG_SITE
USR_CPPFLAGS += $USR_CPPFLAGS
USR_CFLAGS += $USR_CFLAGS
USR_CXXFLAGS += $USR_CXXFLAGS
EOF

make -j2 -C epics-base
