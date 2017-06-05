function Controller() {
	installer.autoAcceptMessageBoxes();
	installer.installationFinished.connect(function() {
		gui.clickButton(buttons.NextButton);
	})
}

Controller.prototype.WelcomePageCallback = function() {
	gui.clickButton(buttons.NextButton);
}

Controller.prototype.IntroductionPageCallback = function() {
	gui.clickButton(buttons.NextButton);
}

Controller.prototype.TargetDirectoryPageCallback = function() {
	gui.clickButton(buttons.NextButton);
}

Controller.prototype.ComponentSelectionPageCallback = function() {
	gui.clickButton(buttons.NextButton);
}

Controller.prototype.StartMenuDirectoryPageCallback = function() {
	gui.clickButton(buttons.NextButton);
}

Controller.prototype.ReadyForInstallationPageCallback = function() {
	gui.clickButton(buttons.NextButton);
}

Controller.prototype.FinishedPageCallback = function() {
	gui.clickButton(buttons.FinishButton);
}

