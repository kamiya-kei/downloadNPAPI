Download NPAPI Plugin for chrome, Windows
=========================================
Google Chrome拡張の為のWindows専用NPAPIプラグインです。  
javascriptからプラグインを呼び出してダウンロード及びその関連機能を利用できます。  

動作確認：windows7 64bit, Google Chrome 26.0.1403.0 dev-m  

■ソースコードのコンパイル
---------------------
ソースコードのコンパイル方法については[readme_src.md](https://github.com/PekeTaichi/downloadNPAPI/blob/master/readme_src.md)を読んでください。

■更新履歴
更新履歴は[history.md](https://github.com/PekeTaichi/downloadNPAPI/blob/master/history.md)を読んでください。

●使い方
--------
**～プラグインの呼び出し～** 
###・manifest.json###
    "plugins" : [
        { "path": "npdownloads.dll", "public": true}
    ],

※実際に使用する際は「,"public":true」は消去推奨

###・htmlファイル###
    <object id="pluginId" type="application/downloads" style="width: 0; height: 0;">
    Plugin FAILED to load
    </object>

###・jsファイル###
    pluginelm = document.getElementById("pluginId");
    dl = pluginelm.Downloads();
    dl.download(url,path,filename,0);

**～各メソッドの説明～**

###・ダウンロードの実行###
    download(url, path, filename, ask);

+   `url` :  
　　 _ダウンロードするファイルのURL_

+   `path` :  
　　 _保存先のパス_

+   `filename` ：  
　　 _保存するファイル名_

+   `ask` ：  
　　 _0=指定したパスに保存, 1=保存先を尋ねる_

+   `戻り値` ：  
　　 _なし_

※pathにファイル名を含めたフルパスを渡し、filenameを省略(空文字)にしてもOKです。  
※ask=1の時、path・filenameを指定することで名前を付けて保存ダイアログのデフォルトパス、ファイル名を指定できます。  
  
※上書き保存する場合は確認ダイアログが呼び出されます。  
　「はい」は上書き保存、「いいえ」は別の名前で保存(名前を付けて保存ダイアログ呼び出し)、「キャンセル」はダウンロード中止です。  
  
※セキュリティ対策の為、スタートアップフォルダ及び環境変数PATHの通ったところには保存できません。  
  
※複数のファイルをダウンロードする場合は必ず下記のようにインスタンスを分けてください。  
ダウンロード状況の確認、キャンセル等が出来なくなります。
###（インスタンスを分ける）####
    var dl1 = pluginelm.Downloads();
    var dl2 = pluginelm.Downloads();
    dl1.download(url1, path1, filename1, 0);
    dl2.download(url2, path2, filename2, 0);

###・指定したURLにPOST(してクッキーを取得)###
    post(url, postmsg);

+   `url` ：  
　　 _POSTしたいURL_

+   `postmsg` ：  
　　 _postするデータ(name=value&name2=value2の形式)_

+   `戻り値` ：  
　　 _クッキーを取得した場合クッキーの文字列を返します_

※POSTするだけでwebデータは取得しません。  
　ヘッダのみ受信し、その際に「Setcookie：～」の記述があればクッキーを設定します。

###・クッキーを追加します(１つ)###
    addCookie(value);

+   `value` ：  
　　 _設定したいクッキー(name=valueの形式)_

###・クッキーを初期化し設定し直します(複数)###
    setCookie(value);

+   `value` ：  
　　 _設定したいクッキー(name=value&name2=value2の形式)_

###・設定されたクッキーを全て削除します(１つ)###
    clearCookie();

###・設定されたクッキーを取得します###
    getCookie();

+   `戻り値` ：  
　　 _設定されたクッキーの文字列(name=value&name2=value2の形式)_

###・リファラを設定します###
    setReferer(value);
+   `value` ：  
　　 _設定したいリファラのURL_

###・設定されたクッキーを取得します###
    getReferer();

+   `戻り値` ：  
　　 _設定されたリファラのURL_

###（例）PIXIVにログインして画像をダウンロードする場合###
    dl = plugin.Downloads();
    dl.post("http://www.pixiv.net/login.php", "pixiv_id=myid&pass=mypass&mode=login");
    dl.setReferer("http://www.pixiv.net/member_illust.php?mode=big&illust_id=(画像ID)");
    dl.download(url, "", "", 1);

###・ダウンロードするファイルのサイズの取得###
    getFileSize();

+   `戻り値` ：  
　　 _ファイルのサイズ(ヘッダのContent-Length :～の部分)_

###・ダウンロード状況の確認(Bytes)###
    getNowSize();

+   `戻り値` ：  
　　 _ファイルのサイズ_

###・ダウンロード状況の確認(％)###
    getNowPer();

+   `戻り値` ：  
　　 _ダウンロード状況をfloat(0～1.0)で返す(上2つを割っただけ)_  
　　 _ファイルサイズ未取得の場合は-1.0_  
　　 _ダウンロード完了済みの場合は2.0を返す_

###・ダウンロードのキャンセル###
    setCancelFlag(1);

###・ダウンロードのエラーの内容の取得###
    getERROR();

+   `戻り値` ：  
　　 _エラーの内容を文字列で取得_  
　　 _"404 Not Found"など_  

###・その他プロパティの取得/設定メソッド###
    setFileSize(value);
    setNowSize(value);
    setNowPer(value);
    getCancelFlag();
    setERROR(value);

**～その他のメソッドの説明～**

###・ディレクトリの存在確認###
    dirExist(path);

+   `path` :  
　　 _ディレクトリのパス_

+   `戻り値` ：  
　　 _1(有)or0(無)_  
　　 _<単にPathIsDirectoryの戻り値を返す>_

###・ディレクトリの生成(再帰的)###
    makeDirs(path);

+   `path` :  
　　 _生成したいディレクトリを含めたフルパス_

+   `戻り値` ：  
　　 _1(成功)or0(失敗)_

###・ファイルの存在確認###
    fileExist(path, filename);

+   `path` :  
　　 _ファイルが存在するディレクトリのパス_

+   `filename` :  
　　 _ファイル名_

+   `戻り値` ：  
　　 _1(成功)or0(失敗)_  
　　 _<単にPathFileExistsの戻り値を返す>_

※filenameを空白文字にしてpathにフルパス指定でも可


###・重複しないファイル名の取得(ファイルの上書き防止用)###
    getNonFilename(path, filename);

+   `path` :  
　　 _ファイルが存在するディレクトリのパス_

+   `filename` :  
　　 _ファイル名_

+   `戻り値` ：  
　　 _ファイル名末尾に(1)や(2),(3)...のように既存ファイルと被らないファイル名を返す_

###・デスクトップのフルパスを取得###
    getDesktop();

+   `戻り値` ：  
　　 _デスクトップのフルパス_  
　　 _<単にSHGetSpecialFolderPath(NULL, Path, CSIDL_DESKTOP, FALSE)のPathを返す>_

###・ディレクトリ選択ダイアログの呼び出し###
    selectDirDialog();

+   `戻り値` ：  
　　選択したディレクトリのフルパス

###・名前を付けて保存ダイアログの呼び出し###
    saveFileDialog(path, filename);

+   `path` :  
　　 _デフォルトのパス(空文字可)_

+   `filename` :  
　　 _デフォルトのファイル名(空文字可)_

+   `戻り値` ：  
　　指定したフルパス

###・警告ダイアログの呼び出し###
    alertDialog(msg);

+   `msg` :  
　　 _警告文_

###・確認ダイアログの呼び出し###
    askDialog(msg, title, flag);

+   `msg` :  
　　 _確認メッセージ_

+   `filename` :  
　　 _ダイアログのタイトル(タイトルバーの文字)_

+   `flag` :  
　　 _1=はい・いいえ・キャンセル, 0=はい・いいえ_

+   `戻り値` ：  
　　_1=はい, 0=いいえ, -1=キャンセル_

###・指定したパスをエクスプローラーで開く###
    openExplorer(path);

+   `path` :  
　　 _開きたいパス_

