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
readme="$destDir/README.md"
doxme="$scriptDir/doxme.py"

python "$doxme" "$srcDir/../README.md"

cat "$doxyTemplate" > Doxyfile
echo "PROJECT_NUMBER = \"$version\"" >> Doxyfile
echo "INPUT += \"$readme\"" >> Doxyfile
echo "USE_MDFILE_AS_MAINPAGE = \"$readme\"" >> Doxyfile
echo "OUTPUT_DIRECTORY = \"$destDir\"" >> Doxyfile
echo "QHP_NAMESPACE = \"de.skycoder42.qtautoupdater.$verTag\"" >> Doxyfile
echo "QHP_CUST_FILTER_NAME = \"AutoUpdater $version\"" >> Doxyfile
echo "QHP_CUST_FILTER_ATTRS = \"qtautoupdater $version\"" >> Doxyfile
echo "QHG_LOCATION = \"$qtBins/qhelpgenerator\"" >> Doxyfile
echo "INCLUDE_PATH += \"$qtHeaders\"" >> Doxyfile
echo "GENERATE_TAGFILE = \"$destDir/qtautoupdater/qtautoupdater.tags\"" >> Doxyfile
if [ "$DOXY_STYLE" ]; then
	echo "HTML_STYLESHEET = \"$DOXY_STYLE\"" >> Doxyfile
fi
if [ "$DOXY_STYLE_EXTRA" ]; then
	echo "HTML_EXTRA_STYLESHEET = \"$DOXY_STYLE_EXTRA\"" >> Doxyfile
fi

for tagFile in $(find "$qtDocs" -name *.tags); do
	if [ $(basename "$tagFile") != "qtautoupdater/.tags" ]; then
		echo "TAGFILES += "$tagFile=https://doc.qt.io/qt-5"" >> Doxyfile
	fi
done

cd "$srcDir"
doxygen "$destDir/Doxyfile"
