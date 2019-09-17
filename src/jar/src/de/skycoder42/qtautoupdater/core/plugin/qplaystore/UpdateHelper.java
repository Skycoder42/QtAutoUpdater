package de.skycoder42.qtautoupdater.core.plugin.qplaystore;

import java.util.List;

import android.content.Context;
import android.content.ComponentName;
import android.content.Intent;
import android.content.IntentSender.SendIntentException;

import android.content.pm.ResolveInfo;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;

import android.net.Uri;

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

	// https://stackoverflow.com/a/28090925/3767076
	public void openInPlay(Context context, AppUpdateInfo info) {
		// you can also use BuildConfig.APPLICATION_ID
		String appId = info.packageName();
		Intent rateIntent = new Intent(Intent.ACTION_VIEW,
			Uri.parse("market://details?id=" + appId));
		boolean marketFound = false;

		// find all applications able to handle our rateIntent
		final List<ResolveInfo> otherApps = context.getPackageManager()
			.queryIntentActivities(rateIntent, 0);
		for (ResolveInfo otherApp: otherApps) {
			// look for Google Play application
			if (otherApp.activityInfo.applicationInfo.packageName
					.equals("com.android.vending")) {

				ActivityInfo otherAppActivity = otherApp.activityInfo;
				ComponentName componentName = new ComponentName(
						otherAppActivity.applicationInfo.packageName,
						otherAppActivity.name
						);
				// make sure it does NOT open in the stack of your activity
				rateIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
				// task reparenting if needed
				rateIntent.addFlags(Intent.FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);
				// if the Google Play was already open in a search result
				//  this make sure it still go to the app page you requested
				rateIntent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
				// this make sure only the Google Play app is allowed to
				// intercept the intent
				rateIntent.setComponent(componentName);
				context.startActivity(rateIntent);
				marketFound = true;
				break;

			}
		}

		// if GP not present on device, open web browser
		if (!marketFound) {
			Intent webIntent = new Intent(Intent.ACTION_VIEW,
				Uri.parse("https://play.google.com/store/apps/details?id="+appId));
			context.startActivity(webIntent);
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

	public void triggerRestart(Context context) {
		PackageManager packageManager = context.getPackageManager();
		Intent intent = packageManager.getLaunchIntentForPackage(context.getPackageName());
		ComponentName componentName = intent.getComponent();
		Intent mainIntent = Intent.makeRestartActivityTask(componentName);
		context.startActivity(mainIntent);
		System.exit(0);
	}
}
