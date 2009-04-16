#!/usr/bin/env bash

# First, make sure all files are being included in the tarball...

cd /tmp

echo "Checking out Seed from Git..."
git clone git+ssh://git.gnome.org/git/seed > /tmp/seed-build.log
cp -r seed seed-svn
cd seed

version=`cat configure.ac | perl -e "while(<>){if(/AC_INIT/){ \\$a = \\$_; \\$a =~ s/.*\s(.*)\)/\\$1/; print \\$a;}}"`

echo "Found seed-$version. Making tarball."

./autogen.sh --enable-gtk-doc >> /tmp/seed-build.log
make dist >> /tmp/seed-build.log

mv "seed-$version.tar.gz" ..

cd ..

rm -rf /tmp/seed

tar -xzf "seed-$version.tar.gz"
diff -rq seed-svn "seed-$version" | grep -v "Common subdirectories" | grep -v "Only in seed-0.5" | grep -v "Files .* differ" | grep -v ".git*" | grep -v ".cvs*" | grep -v "autogen.sh" | grep -v "debian" | grep -v "MAINTAINERS" | grep -v "OPEN_QUESTIONS" | grep -v "tools" | less

echo "Attempting to build and test..."

cd "seed-$version"
./configure >> /tmp/seed-build.log
make -j5 >> /tmp/seed-build.log
PATH="src:$PATH"
make test | less

rm -rf /tmp/seed-svn
rm -rf "/tmp/seed-$version"
rm -rf "/tmp/seed-$version.tar.gz"
