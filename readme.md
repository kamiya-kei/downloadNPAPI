Download NPAPI Plugin for chrome, Windows
=========================================
Google Chrome拡張の為のWindows専用NPAPIプラグインです。  
javascriptからプラグインを呼び出してダウンロード及びその関連機能を利用できます。  

動作確認：windows7 64bit, Google Chrome 26.0.1403.0 dev-m  

■ソースコードのコンパイル
---------------------
ソースコードのコンパイル方法についてはreadme_src.mdを読んでください。

●使い方
--------
**～プラグインの呼び出し～** 
###・manifest.json###
    "plugins" : [
        { "path": "npcomplex.dll", "public": true}
    ],

※実際に使用する際はpublicがtrueだとセキュリティ的に問題なので注意

###・htmlファイル###
    <object id="pluginId" type="application/complex" style="width: 0; height: 0;">
    Plugin FAILED to load
    </object>

###・jsファイル###
    pluginelm = document.getElementById("pluginId");
    c = pluginelm.Complex();
    c.download(url,savepath,filename);

**～各メソッドの説明～**

###・ダウンロードの実行###
    download(url, savepath, filename);

+   `url` :  
　　 _ダウンロードするファイルのURL_

+   `savepath` :  
　　 _保存先のパス_

+   `filename` ：  
　　 _保存するファイル名_

+   `戻り値` ：  
　　 _1(成功)or-1～-10(失敗)_

※複数のファイルをダウンロードする場合は必ず下記のようにインスタンスを分けてください。  
ダウンロード状況の確認、キャンセル等が出来なくなります。
###（インスタンスを分ける）####
    var c1 = pluginelm.Complex();
    var c2 = pluginelm.Complex();
    c1.download(url1, savepath1, filename1);
    c2.download(url2, savepath2, filename2);

###・ダウンロードするファイルのサイズの取得###
    getFileSize();
+   `戻り値` ：  
　　 _ファイルのサイズ(ヘッダのContent-Length :～の部分)_

###・ダウンロード状況の確認(Bytes)###
    getNowSize();
+   `戻り値` ：  
　　 _ファイルのサイズ(256バイト毎に更新)_

###・ダウンロード状況の確認(％)###
    getNowPer();
+   `戻り値` ：  
　　 _ダウンロード状況をfloatで返す(上2つを割っただけ)_  
　　 _ファイルサイズ未取得の場合は-1.0_  
　　 _ダウンロード完了済みの場合は2.0を返す_

###・ダウンロードのキャンセル###
    setCancelFlag(1);

###・用途不明###
    setFileSize(size);
    setNowSize(size);
    setNowPer(size);
    getCancelFlag();

**～その他のメソッドの説明～**

###・ディレクトリの存在確認###
    DirExist(path);

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

###・ファイルの削除###
    deleteFile(path, filename);

+   `path` :  
　　 _ファイルのパス_

+   `filename` :  
　　 _ファイル名_

+   `戻り値` ：  
　　 _1(成功)or0(失敗)_  
　　 _<単にDeleteFileの戻り値を返す>_

※filenameを空白文字にしてpathにフルパス指定でも可

###・ファイルの存在確認###
    FileExist(path, filename);

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

###・ディレクトリ選択ウィンドウの呼び出し###
    selectDir();
+   `戻り値` ：  
　　選択したディレクトリのフルパス

