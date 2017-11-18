#!/bin/bash
# $1: $$SRCDIR
# $2: $$VERSION
# $3: $$[QT_INSTALL_BINS]
# $4: $$[QT_INSTALL_HEADERS]
# $5: $$[QT_INSTALL_DOCS]
# $pwd: dest dir
set -e

scriptDir=$(dirname "$0")
destDir="$(pwd)"
srcDir=$1
version=$2
verTag=$(echo "$version" | sed -e 's/\.//g')
qtBins=$3
qtHeaders=$4
qtDocs=$5
doxyTemplate="$srcDir/Doxyfile"
doxyRes=Doxyfile.generated
readme="$destDir/README.md"
doxme="$scriptDir/doxme.py"

python3 "$doxme" "$srcDir/../README.md"

cat "$doxyTemplate" > $doxyRes
echo "PROJECT_NUMBER = \"$version\"" >> $doxyRes
echo "INPUT += \"$readme\"" >> $doxyRes
echo "USE_MDFILE_AS_MAINPAGE = \"$readme\"" >> $doxyRes
echo "OUTPUT_DIRECTORY = \"$destDir\"" >> $doxyRes
echo "QHP_NAMESPACE = \"de.skycoder42.qtautoupdater.$verTag\"" >> $doxyRes
echo "QHP_CUST_FILTER_NAME = \"AutoUpdater $version\"" >> $doxyRes
echo "QHP_CUST_FILTER_ATTRS = \"qtautoupdater $version\"" >> $doxyRes
echo "QHG_LOCATION = \"$qtBins/qhelpgenerator\"" >> $doxyRes
echo "INCLUDE_PATH += \"$qtHeaders\"" >> $doxyRes
echo "GENERATE_TAGFILE = \"$destDir/qtautoupdater/qtautoupdater.tags\"" >> $doxyRes
if [ "$DOXY_STYLE" ]; then
	echo "HTML_STYLESHEET = \"$DOXY_STYLE\"" >> $doxyRes
fi
if [ "$DOXY_STYLE_EXTRA" ]; then
	echo "HTML_EXTRA_STYLESHEET = \"$DOXY_STYLE_EXTRA\"" >> $doxyRes
fi

for tagFile in $(find "$qtDocs" -name *.tags); do
	if [ $(basename "$tagFile") != "qtautoupdater.tags" ]; then
		echo "TAGFILES += \"$tagFile=https://doc.qt.io/qt-5\"" >> $doxyRes
	fi
done

cd "$srcDir"
doxygen "$destDir/$doxyRes"
