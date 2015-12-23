: arguments:
: %1: path to visual studio binary folder
: %2: %{CurrentBuild:Type}
: %3: lib suffix
: %4: deploy folder name
: working dir: %{buildDir}
@echo off

mkdir libOut\%4\%2

copy .\AutoUpdater\%2\AutoUpdater%3.lib .\libOut\%4\%2\AutoUpdater%3.lib
copy .\AutoUpdater\%2\AutoUpdater%3.pdb .\libOut\%4\%2\AutoUpdater%3.pdb
copy .\AutoUpdaterWidgets\%2\AutoUpdaterWidgets%3.pdb .\libOut\%4\%2\AutoUpdaterWidgets%3.pdb

"%~1\lib.exe" /OUT:libOut\%4\%2\AutoUpdaterWidgets%3.lib .\AutoUpdater\%2\AutoUpdater%3.lib .\AutoUpdaterWidgets\%2\AutoUpdaterWidgets%3.lib
