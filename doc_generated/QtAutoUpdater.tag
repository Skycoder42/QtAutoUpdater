<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile>
  <compound kind="class">
    <name>QtAutoUpdater::AdminAuthoriser</name>
    <filename>class_qt_auto_updater_1_1_admin_authoriser.html</filename>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~AdminAuthoriser</name>
      <anchorfile>class_qt_auto_updater_1_1_admin_authoriser.html</anchorfile>
      <anchor>a71fa6a5ba3cee8eb2f4d095539a74139</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual bool</type>
      <name>hasAdminRights</name>
      <anchorfile>class_qt_auto_updater_1_1_admin_authoriser.html</anchorfile>
      <anchor>af1271ea824ad19a7442745c6cb79a4ac</anchor>
      <arglist>()=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual bool</type>
      <name>executeAsAdmin</name>
      <anchorfile>class_qt_auto_updater_1_1_admin_authoriser.html</anchorfile>
      <anchor>a6d2da8f9983fa25c331394ff82c61034</anchor>
      <arglist>(const QString &amp;program, const QStringList &amp;arguments)=0</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>QtAutoUpdater::BasicLoopUpdateTask</name>
    <filename>class_qt_auto_updater_1_1_basic_loop_update_task.html</filename>
    <base>QtAutoUpdater::LoopUpdateTask</base>
    <member kind="function">
      <type></type>
      <name>BasicLoopUpdateTask</name>
      <anchorfile>class_qt_auto_updater_1_1_basic_loop_update_task.html</anchorfile>
      <anchor>a68a56af476ae19e454d4660c6c39b275</anchor>
      <arglist>(TimeSpan loopDelta, qint64 repeats=-1)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>BasicLoopUpdateTask</name>
      <anchorfile>class_qt_auto_updater_1_1_basic_loop_update_task.html</anchorfile>
      <anchor>a4827e92ce355aae3c48a0f16bf66ded0</anchor>
      <arglist>(const QByteArray &amp;data)</arglist>
    </member>
    <member kind="function">
      <type>qint64</type>
      <name>repetitions</name>
      <anchorfile>class_qt_auto_updater_1_1_basic_loop_update_task.html</anchorfile>
      <anchor>a1acb2341f440ef83a8f56a88e660dc3b</anchor>
      <arglist>() const Q_DECL_OVERRIDE</arglist>
    </member>
    <member kind="function">
      <type>TimeSpan</type>
      <name>pauseSpan</name>
      <anchorfile>class_qt_auto_updater_1_1_basic_loop_update_task.html</anchorfile>
      <anchor>a099f33778fb77078fa9eed4352df76dd</anchor>
      <arglist>() const Q_DECL_OVERRIDE</arglist>
    </member>
    <member kind="function">
      <type>QByteArray</type>
      <name>store</name>
      <anchorfile>class_qt_auto_updater_1_1_basic_loop_update_task.html</anchorfile>
      <anchor>abf5b74a19a1993741fab521c2722e0e2</anchor>
      <arglist>() const Q_DECL_OVERRIDE</arglist>
    </member>
    <member kind="function">
      <type>std::type_index</type>
      <name>typeIndex</name>
      <anchorfile>class_qt_auto_updater_1_1_basic_loop_update_task.html</anchorfile>
      <anchor>a28708485e196b9405caa972b8af0ad41</anchor>
      <arglist>() const Q_DECL_OVERRIDE</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>QtAutoUpdater::LoopUpdateTask</name>
    <filename>class_qt_auto_updater_1_1_loop_update_task.html</filename>
    <base>QtAutoUpdater::UpdateTask</base>
    <member kind="function" virtualness="virtual">
      <type>virtual TimeSpan</type>
      <name>startDelay</name>
      <anchorfile>class_qt_auto_updater_1_1_loop_update_task.html</anchorfile>
      <anchor>a05f5546aebe54a216c8283260dbc4c7e</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual qint64</type>
      <name>repetitions</name>
      <anchorfile>class_qt_auto_updater_1_1_loop_update_task.html</anchorfile>
      <anchor>a9ac64bf7b406d83a46c0bf016e9d562c</anchor>
      <arglist>() const  =0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual TimeSpan</type>
      <name>pauseSpan</name>
      <anchorfile>class_qt_auto_updater_1_1_loop_update_task.html</anchorfile>
      <anchor>a71ac84411139bfea3d23ccb6b5510d10</anchor>
      <arglist>() const  =0</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>hasTasks</name>
      <anchorfile>class_qt_auto_updater_1_1_loop_update_task.html</anchorfile>
      <anchor>a56d55aa565894f743826153fa06030e4</anchor>
      <arglist>() Q_DECL_OVERRIDE</arglist>
    </member>
    <member kind="function">
      <type>QDateTime</type>
      <name>currentTask</name>
      <anchorfile>class_qt_auto_updater_1_1_loop_update_task.html</anchorfile>
      <anchor>a77f564b65c06db8773a4ad348263cc02</anchor>
      <arglist>() const Q_DECL_OVERRIDE</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>nextTask</name>
      <anchorfile>class_qt_auto_updater_1_1_loop_update_task.html</anchorfile>
      <anchor>a2c3ac788456fa50f564833b9207edb6c</anchor>
      <arglist>() Q_DECL_OVERRIDE</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>qint64</type>
      <name>getLeftReps</name>
      <anchorfile>class_qt_auto_updater_1_1_loop_update_task.html</anchorfile>
      <anchor>a19dcc6e798e48ab2d24a6104dbd542cd</anchor>
      <arglist>() const </arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>QtAutoUpdater::TimePointUpdateTask</name>
    <filename>class_qt_auto_updater_1_1_time_point_update_task.html</filename>
    <base>QtAutoUpdater::UpdateTask</base>
    <member kind="function">
      <type></type>
      <name>TimePointUpdateTask</name>
      <anchorfile>class_qt_auto_updater_1_1_time_point_update_task.html</anchorfile>
      <anchor>a5689ef390a2d210333384afe0811482c</anchor>
      <arglist>(const QDateTime &amp;timePoint, TimeSpan::TimeUnit repeatFocus=TimeSpan::MilliSeconds)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TimePointUpdateTask</name>
      <anchorfile>class_qt_auto_updater_1_1_time_point_update_task.html</anchorfile>
      <anchor>a7b1f3f4cabc13d106eaee59d10fadbe2</anchor>
      <arglist>(const QByteArray &amp;data)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>hasTasks</name>
      <anchorfile>class_qt_auto_updater_1_1_time_point_update_task.html</anchorfile>
      <anchor>aeeeb0cdb4bf9640ddfabdf98d4ba1cae</anchor>
      <arglist>() Q_DECL_OVERRIDE</arglist>
    </member>
    <member kind="function">
      <type>QDateTime</type>
      <name>currentTask</name>
      <anchorfile>class_qt_auto_updater_1_1_time_point_update_task.html</anchorfile>
      <anchor>aa9b99b79a3b20eac169d97f159903d83</anchor>
      <arglist>() const Q_DECL_OVERRIDE</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>nextTask</name>
      <anchorfile>class_qt_auto_updater_1_1_time_point_update_task.html</anchorfile>
      <anchor>a95722b1f17cf59d6d27bb61d129b938c</anchor>
      <arglist>() Q_DECL_OVERRIDE</arglist>
    </member>
    <member kind="function">
      <type>QByteArray</type>
      <name>store</name>
      <anchorfile>class_qt_auto_updater_1_1_time_point_update_task.html</anchorfile>
      <anchor>a210037a579f811cdd3ac728e757879cc</anchor>
      <arglist>() const Q_DECL_OVERRIDE</arglist>
    </member>
    <member kind="function">
      <type>std::type_index</type>
      <name>typeIndex</name>
      <anchorfile>class_qt_auto_updater_1_1_time_point_update_task.html</anchorfile>
      <anchor>ac46ae85c056493dbc7b6927bd2176a2b</anchor>
      <arglist>() const Q_DECL_OVERRIDE</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>QtAutoUpdater::TimeSpan</name>
    <filename>struct_qt_auto_updater_1_1_time_span.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>TimeUnit</name>
      <anchorfile>struct_qt_auto_updater_1_1_time_span.html</anchorfile>
      <anchor>a13ac2a0b442eafa547bdc471c728340f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MilliSeconds</name>
      <anchorfile>struct_qt_auto_updater_1_1_time_span.html</anchorfile>
      <anchor>a13ac2a0b442eafa547bdc471c728340fa2810cc25993ecea4db9121bae76ddbe0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Seconds</name>
      <anchorfile>struct_qt_auto_updater_1_1_time_span.html</anchorfile>
      <anchor>a13ac2a0b442eafa547bdc471c728340facf58b9cb5a057891d2cc5cd2db624517</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Minutes</name>
      <anchorfile>struct_qt_auto_updater_1_1_time_span.html</anchorfile>
      <anchor>a13ac2a0b442eafa547bdc471c728340fad85922d8104f0faf56415d499508df0d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Hours</name>
      <anchorfile>struct_qt_auto_updater_1_1_time_span.html</anchorfile>
      <anchor>a13ac2a0b442eafa547bdc471c728340fab83dd5a748e200d9a7310f8e819bf9ea</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Days</name>
      <anchorfile>struct_qt_auto_updater_1_1_time_span.html</anchorfile>
      <anchor>a13ac2a0b442eafa547bdc471c728340fa7cf43d90b77b713efab2660e3e0dedd3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Weeks</name>
      <anchorfile>struct_qt_auto_updater_1_1_time_span.html</anchorfile>
      <anchor>a13ac2a0b442eafa547bdc471c728340fa51787805e34f6d811fbcf8206f0cea18</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Months</name>
      <anchorfile>struct_qt_auto_updater_1_1_time_span.html</anchorfile>
      <anchor>a13ac2a0b442eafa547bdc471c728340faca154c92ad2309dc42197e147e48d953</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Years</name>
      <anchorfile>struct_qt_auto_updater_1_1_time_span.html</anchorfile>
      <anchor>a13ac2a0b442eafa547bdc471c728340fabcd135c707cefe367550fabb4287ad9b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MilliSeconds</name>
      <anchorfile>struct_qt_auto_updater_1_1_time_span.html</anchorfile>
      <anchor>a13ac2a0b442eafa547bdc471c728340fa2810cc25993ecea4db9121bae76ddbe0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Seconds</name>
      <anchorfile>struct_qt_auto_updater_1_1_time_span.html</anchorfile>
      <anchor>a13ac2a0b442eafa547bdc471c728340facf58b9cb5a057891d2cc5cd2db624517</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Minutes</name>
      <anchorfile>struct_qt_auto_updater_1_1_time_span.html</anchorfile>
      <anchor>a13ac2a0b442eafa547bdc471c728340fad85922d8104f0faf56415d499508df0d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Hours</name>
      <anchorfile>struct_qt_auto_updater_1_1_time_span.html</anchorfile>
      <anchor>a13ac2a0b442eafa547bdc471c728340fab83dd5a748e200d9a7310f8e819bf9ea</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Days</name>
      <anchorfile>struct_qt_auto_updater_1_1_time_span.html</anchorfile>
      <anchor>a13ac2a0b442eafa547bdc471c728340fa7cf43d90b77b713efab2660e3e0dedd3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Weeks</name>
      <anchorfile>struct_qt_auto_updater_1_1_time_span.html</anchorfile>
      <anchor>a13ac2a0b442eafa547bdc471c728340fa51787805e34f6d811fbcf8206f0cea18</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Months</name>
      <anchorfile>struct_qt_auto_updater_1_1_time_span.html</anchorfile>
      <anchor>a13ac2a0b442eafa547bdc471c728340faca154c92ad2309dc42197e147e48d953</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Years</name>
      <anchorfile>struct_qt_auto_updater_1_1_time_span.html</anchorfile>
      <anchor>a13ac2a0b442eafa547bdc471c728340fabcd135c707cefe367550fabb4287ad9b</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TimeSpan</name>
      <anchorfile>struct_qt_auto_updater_1_1_time_span.html</anchorfile>
      <anchor>ae65747cf60cfa97671f1a27a9d5cf349</anchor>
      <arglist>(quint64 count=0, TimeUnit unit=MilliSeconds)</arglist>
    </member>
    <member kind="function">
      <type>quint64</type>
      <name>msecs</name>
      <anchorfile>struct_qt_auto_updater_1_1_time_span.html</anchorfile>
      <anchor>a5a32b79b848ccd3f1ec0e1a5cc07f89f</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>QDateTime</type>
      <name>addToDateTime</name>
      <anchorfile>struct_qt_auto_updater_1_1_time_span.html</anchorfile>
      <anchor>a24621de889d3a3618857df777fb41303</anchor>
      <arglist>(const QDateTime &amp;base=QDateTime::currentDateTime()) const </arglist>
    </member>
    <member kind="variable">
      <type>quint64</type>
      <name>count</name>
      <anchorfile>struct_qt_auto_updater_1_1_time_span.html</anchorfile>
      <anchor>af0df7fc925ead228b2d03aa72bef37d6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>enum QtAutoUpdater::TimeSpan::TimeUnit</type>
      <name>unit</name>
      <anchorfile>struct_qt_auto_updater_1_1_time_span.html</anchorfile>
      <anchor>adeb4cf9cc727389a48492969c6d6d4ac</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>QtAutoUpdater::UpdateController</name>
    <filename>class_qt_auto_updater_1_1_update_controller.html</filename>
    <base>QObject</base>
    <member kind="enumeration">
      <type></type>
      <name>DisplayLevel</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a3c3df76116942329e05f639e60bbf5a2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>AutomaticLevel</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a3c3df76116942329e05f639e60bbf5a2a218e6b929fa2dafddd6a22c9428032e5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>ExitLevel</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a3c3df76116942329e05f639e60bbf5a2aa33ac71236ae4be38c5aee9fa71518f1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>InfoLevel</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a3c3df76116942329e05f639e60bbf5a2a43b3e1289758645dc665b719f0742227</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>ExtendedInfoLevel</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a3c3df76116942329e05f639e60bbf5a2a468838bf2f9bb49c6c135f1402623f77</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>ProgressLevel</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a3c3df76116942329e05f639e60bbf5a2a0afbb2b505b5e446b5d0209f36e8ae10</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>AskLevel</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a3c3df76116942329e05f639e60bbf5a2a4478273347d97859fd591b5c302ecc54</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>AutomaticLevel</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a3c3df76116942329e05f639e60bbf5a2a218e6b929fa2dafddd6a22c9428032e5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>ExitLevel</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a3c3df76116942329e05f639e60bbf5a2aa33ac71236ae4be38c5aee9fa71518f1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>InfoLevel</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a3c3df76116942329e05f639e60bbf5a2a43b3e1289758645dc665b719f0742227</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>ExtendedInfoLevel</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a3c3df76116942329e05f639e60bbf5a2a468838bf2f9bb49c6c135f1402623f77</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>ProgressLevel</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a3c3df76116942329e05f639e60bbf5a2a0afbb2b505b5e446b5d0209f36e8ae10</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>AskLevel</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a3c3df76116942329e05f639e60bbf5a2a4478273347d97859fd591b5c302ecc54</anchor>
      <arglist></arglist>
    </member>
    <member kind="slot">
      <type>bool</type>
      <name>start</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a393ea9c15ea826922c76bc871f1ad1aa</anchor>
      <arglist>(DisplayLevel displayLevel=ProgressLevel)</arglist>
    </member>
    <member kind="slot">
      <type>bool</type>
      <name>cancelUpdate</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a27c3ec1b1274b11788524a49c7137591</anchor>
      <arglist>(int maxDelay=3000)</arglist>
    </member>
    <member kind="slot">
      <type>int</type>
      <name>scheduleUpdate</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>ab1da1140579c77dcaad87d1ecbc0771a</anchor>
      <arglist>(qint64 delayMinutes, bool repeated=false, DisplayLevel displayLevel=InfoLevel)</arglist>
    </member>
    <member kind="slot">
      <type>int</type>
      <name>scheduleUpdate</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>ad838c16546b206df338137d8d0bd6ab4</anchor>
      <arglist>(const QDateTime &amp;when, DisplayLevel displayLevel=InfoLevel)</arglist>
    </member>
    <member kind="slot">
      <type>int</type>
      <name>scheduleUpdate</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>aa0de51652fd8c42442a029811c39c4e8</anchor>
      <arglist>(UpdateTask *task, DisplayLevel displayLevel=InfoLevel)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>cancelScheduledUpdate</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a0f91fab099be5305ff435c9828b72178</anchor>
      <arglist>(int taskId)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>runningChanged</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a652a18702e98bf3d8bcb8af4293b6198</anchor>
      <arglist>(bool running)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>runAsAdminChanged</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a52524eb5edae5f4f73cb7f11c1068772</anchor>
      <arglist>(bool runAsAdmin)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>UpdateController</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a79768eee4eb12efbdc52b15cb7170802</anchor>
      <arglist>(QObject *parent=NULL)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>UpdateController</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a3ccd3630f271f0af7a7a031acfefc7ac</anchor>
      <arglist>(QWidget *parentWidget)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>UpdateController</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a5b4310db6a993341947cf19ef7a13c08</anchor>
      <arglist>(const QString &amp;maintenanceToolPath, QObject *parent=NULL)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>UpdateController</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a09292d50ed763e6e25f17ba0436a86e1</anchor>
      <arglist>(const QString &amp;maintenanceToolPath, QWidget *parentWidget)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~UpdateController</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>abacdda249401974823d69bb5024e495d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>QAction *</type>
      <name>getUpdateAction</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a0716b4d92ec66f13cdd5b4a176122a08</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>QWidget *</type>
      <name>createUpdatePanel</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>ae7e9db4151f70d6f8ece97879db32658</anchor>
      <arglist>(QWidget *parentWidget)</arglist>
    </member>
    <member kind="function">
      <type>QString</type>
      <name>maintenanceToolPath</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a03e6a8f8954064bde910aee6d87a1b11</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>DisplayLevel</type>
      <name>currentDisplayLevel</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a8594aa5086925be4c96203b44b0fdd54</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isRunning</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a83c017bdbfa9f5aafba1814dd03314f9</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>runAsAdmin</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>aaefbe486ef930596e9bc0cc0225537ce</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setRunAsAdmin</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>ac0816c7d65dcbc47878360940062c64d</anchor>
      <arglist>(bool runAsAdmin, bool userEditable=true)</arglist>
    </member>
    <member kind="function">
      <type>QStringList</type>
      <name>updateRunArgs</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a1d1f854857c3efed3f2cae28c221b2d5</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setUpdateRunArgs</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a12d10f4df120aeff4e626aa1c20a960f</anchor>
      <arglist>(QStringList updateRunArgs)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>resetUpdateRunArgs</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a0ed0142907a599ba3e2664ad1f5c4927</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>const Updater *</type>
      <name>getUpdater</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a65e94b952ae4ba82138ac38f49bd99ea</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>QWidget *</type>
      <name>parentWidget</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a8d144708b57a05c2f38603dc3b2b9ec2</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setParent</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a78fc64b53328a5339276144673ed81b8</anchor>
      <arglist>(QWidget *parent)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setParent</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a8d02b19f113e3f2c8815ace90dda0e89</anchor>
      <arglist>(QObject *parent)</arglist>
    </member>
    <member kind="property">
      <type>QString</type>
      <name>maintenanceToolPath</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>af5e95a106017414f0b78cf81ef6c364f</anchor>
      <arglist></arglist>
    </member>
    <member kind="property">
      <type>bool</type>
      <name>running</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a7fa2f115bc28f412003822cc0dfdb3d0</anchor>
      <arglist></arglist>
    </member>
    <member kind="property">
      <type>bool</type>
      <name>runAsAdmin</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a1c5a18c8728e9803a1e3ed927f038a53</anchor>
      <arglist></arglist>
    </member>
    <member kind="property">
      <type>QStringList</type>
      <name>updateRunArgs</name>
      <anchorfile>class_qt_auto_updater_1_1_update_controller.html</anchorfile>
      <anchor>a6c6a5e6092b18483497e7fb7f6b5c8ec</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>QtAutoUpdater::Updater::UpdateInfo</name>
    <filename>struct_qt_auto_updater_1_1_updater_1_1_update_info.html</filename>
    <member kind="function">
      <type></type>
      <name>UpdateInfo</name>
      <anchorfile>struct_qt_auto_updater_1_1_updater_1_1_update_info.html</anchorfile>
      <anchor>aea1b47276eec750f1357c4a96d7dc546</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>UpdateInfo</name>
      <anchorfile>struct_qt_auto_updater_1_1_updater_1_1_update_info.html</anchorfile>
      <anchor>a8fcd3743ce4846a15667d84f0be3d85e</anchor>
      <arglist>(const UpdateInfo &amp;other)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>UpdateInfo</name>
      <anchorfile>struct_qt_auto_updater_1_1_updater_1_1_update_info.html</anchorfile>
      <anchor>acb4f2b04b4304d5bfc5063db38936b05</anchor>
      <arglist>(QString name, QVersionNumber version, quint64 size)</arglist>
    </member>
    <member kind="variable">
      <type>QString</type>
      <name>name</name>
      <anchorfile>struct_qt_auto_updater_1_1_updater_1_1_update_info.html</anchorfile>
      <anchor>a28760ca994913fe3c19f34e0f4f25c21</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>QVersionNumber</type>
      <name>version</name>
      <anchorfile>struct_qt_auto_updater_1_1_updater_1_1_update_info.html</anchorfile>
      <anchor>abcad59ff52bcc1f7fa500a053095bb2b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>quint64</type>
      <name>size</name>
      <anchorfile>struct_qt_auto_updater_1_1_updater_1_1_update_info.html</anchorfile>
      <anchor>a526ebb1045a57eefb2287906a3b82c8c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>QtAutoUpdater::Updater</name>
    <filename>class_qt_auto_updater_1_1_updater.html</filename>
    <base>QObject</base>
    <class kind="struct">QtAutoUpdater::Updater::UpdateInfo</class>
    <member kind="slot">
      <type>bool</type>
      <name>checkForUpdates</name>
      <anchorfile>class_qt_auto_updater_1_1_updater.html</anchorfile>
      <anchor>a104b10bae48235afce2826ab1385c0b9</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>abortUpdateCheck</name>
      <anchorfile>class_qt_auto_updater_1_1_updater.html</anchorfile>
      <anchor>acebfb662db8ab87c89dbaf96d61d131c</anchor>
      <arglist>(int maxDelay=5000, bool async=false)</arglist>
    </member>
    <member kind="slot">
      <type>int</type>
      <name>scheduleUpdate</name>
      <anchorfile>class_qt_auto_updater_1_1_updater.html</anchorfile>
      <anchor>a81e939b61f337437cdb1d2fdb1d03e36</anchor>
      <arglist>(qint64 delayMinutes, bool repeated=false)</arglist>
    </member>
    <member kind="slot">
      <type>int</type>
      <name>scheduleUpdate</name>
      <anchorfile>class_qt_auto_updater_1_1_updater.html</anchorfile>
      <anchor>ae3653cd691f5966e26b5ea6b5f71d902</anchor>
      <arglist>(const QDateTime &amp;when)</arglist>
    </member>
    <member kind="slot">
      <type>int</type>
      <name>scheduleUpdate</name>
      <anchorfile>class_qt_auto_updater_1_1_updater.html</anchorfile>
      <anchor>a4d966e203ac7261ea28ac5231eeb1c4e</anchor>
      <arglist>(UpdateTask *task)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>cancelScheduledUpdate</name>
      <anchorfile>class_qt_auto_updater_1_1_updater.html</anchorfile>
      <anchor>a62affe5f6d482482b239b7d3822657dd</anchor>
      <arglist>(int taskId)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>runUpdaterOnExit</name>
      <anchorfile>class_qt_auto_updater_1_1_updater.html</anchorfile>
      <anchor>aa85d5f18c68075b24366d78fac781686</anchor>
      <arglist>(AdminAuthoriser *authoriser=NULL)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>runUpdaterOnExit</name>
      <anchorfile>class_qt_auto_updater_1_1_updater.html</anchorfile>
      <anchor>a262b3fe8de198746af8d43243f8f9a04</anchor>
      <arglist>(const QStringList &amp;arguments, AdminAuthoriser *authoriser=NULL)</arglist>
    </member>
    <member kind="slot">
      <type>void</type>
      <name>cancelExitRun</name>
      <anchorfile>class_qt_auto_updater_1_1_updater.html</anchorfile>
      <anchor>ac5320a29ffba285d40100a1e98e8950a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>checkUpdatesDone</name>
      <anchorfile>class_qt_auto_updater_1_1_updater.html</anchorfile>
      <anchor>adcbda9d4e218cd497027b1d3fed3d5b5</anchor>
      <arglist>(bool hasUpdates, bool hasError=false)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>runningChanged</name>
      <anchorfile>class_qt_auto_updater_1_1_updater.html</anchorfile>
      <anchor>a95be75ea3c2c917cf2b7d1006d1eff93</anchor>
      <arglist>(bool running)</arglist>
    </member>
    <member kind="signal">
      <type>void</type>
      <name>updateInfoChanged</name>
      <anchorfile>class_qt_auto_updater_1_1_updater.html</anchorfile>
      <anchor>ad8911bff3cf527089eed208340686c0c</anchor>
      <arglist>(QList&lt; QtAutoUpdater::Updater::UpdateInfo &gt; updateInfo)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Updater</name>
      <anchorfile>class_qt_auto_updater_1_1_updater.html</anchorfile>
      <anchor>a78c4bf8ef0ac1a9720a087cc9d441512</anchor>
      <arglist>(QObject *parent=NULL)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Updater</name>
      <anchorfile>class_qt_auto_updater_1_1_updater.html</anchorfile>
      <anchor>a4f6b269555a477a0039d3aac638df007</anchor>
      <arglist>(const QString &amp;maintenanceToolPath, QObject *parent=NULL)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~Updater</name>
      <anchorfile>class_qt_auto_updater_1_1_updater.html</anchorfile>
      <anchor>a1294129a47405b4e29932555bb9cdf43</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>exitedNormally</name>
      <anchorfile>class_qt_auto_updater_1_1_updater.html</anchorfile>
      <anchor>a53b3f8154cd19b46f8d97c9c1ca5701a</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getErrorCode</name>
      <anchorfile>class_qt_auto_updater_1_1_updater.html</anchorfile>
      <anchor>a61ecb06ecbd80dab0931f3c78b458db5</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>QByteArray</type>
      <name>getErrorLog</name>
      <anchorfile>class_qt_auto_updater_1_1_updater.html</anchorfile>
      <anchor>ae25e2214233714b3ed0383aae8841be5</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>willRunOnExit</name>
      <anchorfile>class_qt_auto_updater_1_1_updater.html</anchorfile>
      <anchor>a0c81b1d6d66a3049a46f8cd49ed82937</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>QString</type>
      <name>maintenanceToolPath</name>
      <anchorfile>class_qt_auto_updater_1_1_updater.html</anchorfile>
      <anchor>af6267596d51b9224fedb4108f1e8f9af</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isRunning</name>
      <anchorfile>class_qt_auto_updater_1_1_updater.html</anchorfile>
      <anchor>a59f0da8efb9bdf94f8c01768b7e14298</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>QList&lt; UpdateInfo &gt;</type>
      <name>updateInfo</name>
      <anchorfile>class_qt_auto_updater_1_1_updater.html</anchorfile>
      <anchor>a43ac62f5fc35cd3ff608aaa74452e770</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="property">
      <type>QString</type>
      <name>maintenanceToolPath</name>
      <anchorfile>class_qt_auto_updater_1_1_updater.html</anchorfile>
      <anchor>a461606ef021a435745a1e3bdfb422b05</anchor>
      <arglist></arglist>
    </member>
    <member kind="property">
      <type>bool</type>
      <name>running</name>
      <anchorfile>class_qt_auto_updater_1_1_updater.html</anchorfile>
      <anchor>aef607db5109f57300e47b8e35615c7fa</anchor>
      <arglist></arglist>
    </member>
    <member kind="property">
      <type>QList&lt; UpdateInfo &gt;</type>
      <name>updateInfo</name>
      <anchorfile>class_qt_auto_updater_1_1_updater.html</anchorfile>
      <anchor>aa3b025dad1bae7d7ac2f7ade23955fc4</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>QtAutoUpdater::UpdateSchedulerController</name>
    <filename>class_qt_auto_updater_1_1_update_scheduler_controller.html</filename>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>registerStoredTask</name>
      <anchorfile>class_qt_auto_updater_1_1_update_scheduler_controller.html</anchorfile>
      <anchor>a273159e2a958237f68ed1f3b94ec3450</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>setSettingsGroup</name>
      <anchorfile>class_qt_auto_updater_1_1_update_scheduler_controller.html</anchorfile>
      <anchor>ad12a75ddf60d84e718967d08e906a0df</anchor>
      <arglist>(const QString &amp;group)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>setSettingsObject</name>
      <anchorfile>class_qt_auto_updater_1_1_update_scheduler_controller.html</anchorfile>
      <anchor>ade023a82b326926c888cde71323e2150</anchor>
      <arglist>(QSettings *settingsObject)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>QtAutoUpdater::UpdateTask</name>
    <filename>class_qt_auto_updater_1_1_update_task.html</filename>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~UpdateTask</name>
      <anchorfile>class_qt_auto_updater_1_1_update_task.html</anchorfile>
      <anchor>a32baf4590167659c502f1b8d0d4fa5ba</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual bool</type>
      <name>hasTasks</name>
      <anchorfile>class_qt_auto_updater_1_1_update_task.html</anchorfile>
      <anchor>a313b3a9bcf5db91833ebb4a672ec348c</anchor>
      <arglist>()=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual QDateTime</type>
      <name>currentTask</name>
      <anchorfile>class_qt_auto_updater_1_1_update_task.html</anchorfile>
      <anchor>af9071665bb71a580f16d39b8ce6b76f1</anchor>
      <arglist>() const  =0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual bool</type>
      <name>nextTask</name>
      <anchorfile>class_qt_auto_updater_1_1_update_task.html</anchorfile>
      <anchor>ab146b9a23674cdddb9c4e45ed93d43f4</anchor>
      <arglist>()=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual std::type_index</type>
      <name>typeIndex</name>
      <anchorfile>class_qt_auto_updater_1_1_update_task.html</anchorfile>
      <anchor>a4ef311a95cd34b534bcc585441052a7b</anchor>
      <arglist>() const  =0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual QByteArray</type>
      <name>store</name>
      <anchorfile>class_qt_auto_updater_1_1_update_task.html</anchorfile>
      <anchor>a9f7c07bae15e2bc0df400bbb741451b4</anchor>
      <arglist>() const  =0</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>QtAutoUpdater::UpdateTaskList</name>
    <filename>class_qt_auto_updater_1_1_update_task_list.html</filename>
    <base>QLinkedList&lt; UpdateTask * &gt;</base>
    <base>QtAutoUpdater::UpdateTask</base>
    <member kind="function">
      <type></type>
      <name>UpdateTaskList</name>
      <anchorfile>class_qt_auto_updater_1_1_update_task_list.html</anchorfile>
      <anchor>af4459b10ddd246115cfea2eb2338f0f8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>UpdateTaskList</name>
      <anchorfile>class_qt_auto_updater_1_1_update_task_list.html</anchorfile>
      <anchor>a64bcfa5d2fc69b68fe8733f16443d8fb</anchor>
      <arglist>(const std::initializer_list&lt; UpdateTask * &gt; &amp;list)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>UpdateTaskList</name>
      <anchorfile>class_qt_auto_updater_1_1_update_task_list.html</anchorfile>
      <anchor>ab69f4f2437403066853e69954c1f792e</anchor>
      <arglist>(const QByteArray &amp;data)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>hasTasks</name>
      <anchorfile>class_qt_auto_updater_1_1_update_task_list.html</anchorfile>
      <anchor>a12c73abbe2d396db921c50cc6b7b3753</anchor>
      <arglist>() Q_DECL_OVERRIDE</arglist>
    </member>
    <member kind="function">
      <type>QDateTime</type>
      <name>currentTask</name>
      <anchorfile>class_qt_auto_updater_1_1_update_task_list.html</anchorfile>
      <anchor>a88473d11533aae5926851991e3eff801</anchor>
      <arglist>() const Q_DECL_OVERRIDE</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>nextTask</name>
      <anchorfile>class_qt_auto_updater_1_1_update_task_list.html</anchorfile>
      <anchor>a941ed9869bcd22a568ff5c9b8ca6849e</anchor>
      <arglist>() Q_DECL_OVERRIDE</arglist>
    </member>
    <member kind="function">
      <type>QByteArray</type>
      <name>store</name>
      <anchorfile>class_qt_auto_updater_1_1_update_task_list.html</anchorfile>
      <anchor>ae8321a04b03ad85a81d201da082a00e7</anchor>
      <arglist>() const Q_DECL_OVERRIDE</arglist>
    </member>
    <member kind="function">
      <type>std::type_index</type>
      <name>typeIndex</name>
      <anchorfile>class_qt_auto_updater_1_1_update_task_list.html</anchorfile>
      <anchor>a548e1ded30b8ea67946d7bb7025c5f71</anchor>
      <arglist>() const Q_DECL_OVERRIDE</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>QtAutoUpdater</name>
    <filename>namespace_qt_auto_updater.html</filename>
    <class kind="class">QtAutoUpdater::AdminAuthoriser</class>
    <class kind="class">QtAutoUpdater::BasicLoopUpdateTask</class>
    <class kind="class">QtAutoUpdater::LoopUpdateTask</class>
    <class kind="class">QtAutoUpdater::TimePointUpdateTask</class>
    <class kind="class">QtAutoUpdater::TimeSpan</class>
    <class kind="class">QtAutoUpdater::UpdateController</class>
    <class kind="class">QtAutoUpdater::Updater</class>
    <class kind="class">QtAutoUpdater::UpdateSchedulerController</class>
    <class kind="class">QtAutoUpdater::UpdateTask</class>
    <class kind="class">QtAutoUpdater::UpdateTaskList</class>
  </compound>
  <compound kind="page">
    <name>image_page</name>
    <title>Image Page</title>
    <filename>image_page</filename>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title></title>
    <filename>index</filename>
  </compound>
</tagfile>
