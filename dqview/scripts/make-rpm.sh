#!/bin/sh

# This script must be run from the repo top directory!

mkdir -p ~/rpmbuild/{BUILD,BUILDROOT,RPMS,SOURCES,SPECS,SRPMS}
git archive --prefix dqview/ -o dqview.tar.gz HEAD
mv dqview.tar.gz ~/rpmbuild/SOURCES
QA_RPATHS=0x0001 rpmbuild -ba dqview/dqview.spec
cp ~/rpmbuild/RPMS/`uname -m`/dqview* .
rm -rf ~/rpmbuild
