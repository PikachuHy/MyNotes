package cn.net.pikachu;

import android.app.ActivityManager;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.os.Bundle;
import android.util.Log;

import org.qtproject.qt.android.bindings.QtActivity;

import java.util.List;

public class NoteActivity extends QtActivity {
    private static String TAG = "NoteActivity";
    public NoteActivity() {
        instance = this;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    protected void onStart() {
        super.onStart();
        instance = this;
        ActivityManager systemService = (ActivityManager) instance.getSystemService(Context.ACTIVITY_SERVICE);
        m_packageManager = instance.getPackageManager();
        Intent intent = new Intent(Intent.ACTION_MAIN, null);
        intent.addCategory(Intent.CATEGORY_LAUNCHER);
        List<ResolveInfo> activities = m_packageManager.queryIntentActivities(intent, 0);
        for (int i = 0; i < activities.size(); i++) {
            ResolveInfo info = activities.get(i);
            String name = info.loadLabel(m_packageManager).toString();
            String packageName = info.activityInfo.packageName;
            Log.d(TAG, "onStart: " + name + " " + packageName);
        }
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
    }
    private PackageManager m_packageManager;
    private NoteActivity instance;
}
