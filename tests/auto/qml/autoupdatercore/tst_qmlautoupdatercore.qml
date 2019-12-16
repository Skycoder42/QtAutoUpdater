import QtQml 2.13
import QtQuick 2.13
import QtTest 1.13
import Qt.labs.settings 1.1
import de.skycoder42.QtAutoUpdater.Core 3.0

Item {
	id: root

	TestCase {
		id: testSingleton
		name: "QtAutoUpdaterSingleton"

		Settings {
			id: testSettings

			property string backend: "test"
		}

		function test_createInfo() {
			let info = QtAutoUpdater.createInfo(42, "test-package", "1.0.1", {
													key: "value"
												});
			compare(info.identifier, 42);
			compare(info.name, "test-package");
			compare(info.version, "1.0.1");
			compare(info.data, {
						key: "value"
					});
		}

		function test_infoList() {
			let info = QtAutoUpdater.createInfo(42, "test-package", "1.0.1", {
													key: "value"
												});
			let tList = setup.createInfoList(info);
			let resList = QtAutoUpdater.infosAsList(tList);  // TODO remove deprecated
			compare(resList.length, 1);
			compare(resList[0], info);
		}

		function test_createUpdater() {
			let updater = QtAutoUpdater.createUpdater("test", {
													  }, testSingleton);
			verify(updater);
			compare(updater.backend().key(), "test");

			updater = QtAutoUpdater.createUpdater(srcDir + "test.conf", testSingleton);
			verify(updater);
			compare(updater.backend().key(), "test");

			updater = QtAutoUpdater.createUpdater(testSettings, testSingleton);
			verify(updater);
			compare(updater.backend().key(), "test");
		}
	}
}
