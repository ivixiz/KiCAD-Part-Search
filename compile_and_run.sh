cd $(pwd)/src;
qmake;
make;
chmod +x $(pwd)/part_search;
./../part_search;
