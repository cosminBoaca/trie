cd ./libs/trie/test/
rm -rf bin
b2 | grep -E 'passed|failed|error' --colour=auto
cd ../../../

