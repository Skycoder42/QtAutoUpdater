package de.skycoder42.qtautoupdater.core.plugin.qplaystore;

import android.content.Context;
import android.content.IntentSender.SendIntentException;

import android.app.Activity;

import com.google.android.play.core.tasks.OnCompleteListener;
import com.google.android.play.core.tasks.OnSuccessListener;
import com.google.android.play.core.tasks.Task;

import com.google.android.play.core.appupdate.AppUpdateInfo;
import com.google.android.play.core.appupdate.AppUpdateManager;

import com.google.android.play.core.install.InstallStateUpdatedListener;
import com.google.android.play.core.install.InstallState;

import com.google.android.play.core.install.model.UpdateAvailability;
import com.google.android.play.core.install.model.AppUpdateType;
import com.google.android.play.core.install.model.InstallStatus;

class UpdateHelper implements InstallStateUpdatedListener
{
	private String _id;
	private AppUpdateManager _manager;

	private native void reportCheckResult(AppUpdateInfo info);
	@Override
	public native void onStateUpdate(InstallState state);

	public UpdateHelper(String id, AppUpdateManager manager) {
		_id = id;
		_manager = manager;
	}

	public String id() {
		return _id;
	}

	public void startUpdateCheck() {
		_manager.getAppUpdateInfo().addOnCompleteListener(new OnCompleteListener<AppUpdateInfo>() {
			@Override
			public void onComplete(Task<AppUpdateInfo> task) {
				if (task.isSuccessful())
					reportCheckResult(task.getResult());
				else
					reportCheckResult(null);
			}
		});
	}

	public void resumeStalledUpdate(final int requestCode, final Activity activity) {
		_manager.getAppUpdateInfo().addOnSuccessListener(new OnSuccessListener<AppUpdateInfo>() {
			@Override
			public void onSuccess(AppUpdateInfo info) {
				if (info.updateAvailability() == UpdateAvailability.DEVELOPER_TRIGGERED_UPDATE_IN_PROGRESS)
					triggerUpdate(requestCode, activity, info);
			}
		});
	}

	public boolean triggerUpdate(int requestCode, Activity activity, AppUpdateInfo info) {
		try {
			_manager.startUpdateFlowForResult(info,
											  AppUpdateType.IMMEDIATE,
											  activity,
											  requestCode);
			return true;
		} catch(SendIntentException e) {
			e.printStackTrace();
			return false;
		}
	}

	public boolean startUpdate(int requestCode, Activity activity, AppUpdateInfo info) {
		try {
			_manager.registerListener(this);
			_manager.startUpdateFlowForResult(info,
											  AppUpdateType.FLEXIBLE,
											  activity,
											  requestCode);
			return true;
		} catch(SendIntentException e) {
			_manager.unregisterListener(this);
			e.printStackTrace();
			return false;
		}
	}

	public void completeUpdate() {
		_manager.completeUpdate();
	}
}
