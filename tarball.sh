#/bin/bash
mkdir 311232268
cp -a $1 311232268
tar -cvf 311232268.tar --exclude .git --exclude .gitignore --exclude .vscode --gzip ./311232268
rm -rf 311232268/