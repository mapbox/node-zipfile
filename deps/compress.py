# hg clone http://hg.nih.at/libzip
# mv libzip libzip-0.11.1
# python ./compress.py libzip-0.11.1-mod2.tar.gz libzip-0.11.1
import sys
import tarfile
import os

tarball = os.path.abspath(sys.argv[1])
dirname = sys.argv[2]
tfile = tarfile.open(tarball,'w:gz')
tfile.add(dirname)
tfile.close()
sys.exit(0)
