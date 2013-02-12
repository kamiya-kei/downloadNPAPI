#include <stdio.h>
#include <winsock2.h>
#include <Windows.h>

#include <shlobj.h>
#include <shlwapi.h>
#include <vector>

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

//using namespace std;

#pragma comment(lib, "shlwapi.lib" )
#pragma once
#define BUF_SIZE 64

//プラグイン名もクラス名もサンプルまんま。後で変える。
class Complex {
public:
	Complex() {
		setFileSize(-1);
		setNowSize(0);
		setNowPer(0);
		setCancelFlag(0);		
	}

	int download(std::string url, std::string savepath, std::string filename = ""){
    	char *domain;
    	char *path;
    	int port;
    	std::string p_domain;
    	std::string _domain;
    	std::string _path;

    	WSADATA wd;
    	SOCKET sock;
    	struct sockaddr_in server;
    	char buf[BUF_SIZE];
    	unsigned int **addrptr;

		if (filename != ""){
			savepath += "\\" + filename;//デフォルト引数はnpapiでは使えないっぽい？
		}

    	//URL解析(プロトコル、ドメイン、パス、ポートの取得)
    	int n= url.find("://");
    	if (n == -1) {
    		std::cout << "not found ://" << std::endl;
    		return -1;
    	}
    	p_domain = url.substr(n+3);
    	n = p_domain.find("/");
    	if (n == -1) {
    		std::cout << "not found /" << std::endl;
    		return -1;
    	}
    	_domain = p_domain.substr(0, n);
    	_path = p_domain.substr(n);
    	path = const_cast<char*>(_path.c_str());
    	n = _domain.find(":");
    	if (n == -1) {
    		domain = const_cast<char*>(_domain.c_str());
    		port = 80;
    	} else {
    		std::string tmp = _domain.substr(0, n);
    		domain = const_cast<char*>(tmp.c_str());
    		port = atoi(_domain.substr(n + 1).c_str());
    	}
    	//URL解析ここまで

    	if ( WSAStartup(MAKEWORD(2,0), &wd) ) {//winsock初期化
    		//初期化失敗
    		std::cout << "WSAStartup failed\n" <<  std::endl;
    		return -2;
    	}

    	sock = socket(AF_INET, SOCK_STREAM, 0);//ソケットの作成
    	if (sock == INVALID_SOCKET) {
    		//作成失敗
    		std::cout << "socket : " << WSAGetLastError() << std::endl;
			WSACleanup();
    		return -3;
    	}
    	//接続先指定用構造体の準備
    	server.sin_family = AF_INET;//インターネット(TCP, UDP, etc...)
    	server.sin_port = htons(port);//httpポート80
    	server.sin_addr.S_un.S_addr = inet_addr(domain);//URL設定(inet_addrは32bitバイナリへの変換)
    	if (server.sin_addr.S_un.S_addr == 0xffffffff) {
    		//URLがIP表記じゃない場合(名前解決が必要な場合)
    		struct hostent *host;
    		host = gethostbyname(domain);//名前解決(→IP)
    		if (host == NULL) {
    			//解決失敗
    			if (WSAGetLastError() == WSAHOST_NOT_FOUND) {
    				std::cout << "host not found : " << domain << std::endl;
    			}else {
    				std::cout << "gethostbyname failed" << std::endl;
    			}
				closesocket(sock);
				WSACleanup();
    			return -4;
    		}
    		addrptr = (unsigned int **)host->h_addr_list;
    		while (*addrptr != NULL) {
    			//1つの名前に複数のIPが関連付けられていることがあるので接続が成功するまで全て試す
    			server.sin_addr.S_un.S_addr = *(*addrptr);
    			if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == 0) {
    				break;//接続成功
    			}
    			addrptr++;
    		}
    		if (*addrptr == NULL) {
    			//全てのIPで接続が失敗
    			std::cout << "connect : " << WSAGetLastError() << std::endl;
				closesocket(sock);
				WSACleanup();
    			return -5;
    		}
    		
    	}

    	//サーバーに接続
    	connect(sock, (struct sockaddr *)&server, sizeof(server));

    	//HTTPリクエスト送信
    	memset(buf, 0, sizeof(buf));//buf初期化(ゼロクリア)
    	sprintf_s(buf, sizeof(buf), "GET %s HTTP/1.1\r\n", path);
    	n = send(sock, buf, (int)strlen(buf), 0);
    	if (n < 0) {
    		//送信失敗
    		std::cout << "send ; " << WSAGetLastError() << std::endl;
			closesocket(sock);
			WSACleanup();
    		return -6;
    	}
    	sprintf_s(buf, sizeof(buf), "host: %s\r\n", domain);
    	n = send(sock, buf, (int)strlen(buf), 0);
    	if (n < 0) {
    		std::cout << "send ; " << WSAGetLastError() << std::endl;
			closesocket(sock);
			WSACleanup();
			return -7;
    	}
    	sprintf_s(buf, sizeof(buf), "\r\n");
    	n = send(sock, buf, (int)strlen(buf), 0);
    	if (n < 0) {
    	std::cout << "send ; " << WSAGetLastError() << std::endl;
			closesocket(sock);
			WSACleanup();
    		return -8;
    	}
		//一気にsend出来ないかな？要検証
		//【メモ】
		//リクエストする内容を引数で受け取って送れるようにする

    	//サーバーからデータ受信(ヘッダ)
    	std::string source = "";
    	while (1) {
    		//cに1文字ずつ取得してsourceに追記して改行が来たらsourceクリア
    		char c;
    		n = recv(sock, &c, 1, 0);
    		if (n == SOCKET_ERROR) {
    			//受信失敗
    			std::cout << "recv : " << WSAGetLastError() << std::endl;
				closesocket(sock);
				WSACleanup();
    			return -9;
    		}
    		if (c == '\n'){
    			//1行毎の処理(sourceに1行のデータが格納されている)
    			if (source.size() < 2) {//source.empty()じゃダメだった。空行なのに1バイト？？
    				break;//ヘッダ終了
    			} 
    			//ファイルサイズ取得
    			int k = source.find("Content-Length: ");
    			if (k != -1){
    				std::string _fsize = source.substr(k + 16);
    				setFileSize(atoi(_fsize.c_str()));
    			}
    			std::cout << source << std::endl;
    			std::cout << "------" << source.length() << "--------------" << std::endl;
    			source = "";//次の行を取得するためにクリア
    			continue;
    		}
    		source += c;//改行が来るまでsourceに蓄積
    	}

    	//サーバーからデータ受信(実データ)
		std::ofstream f;
    	f.open(savepath, std::ios::out|std::ios::binary);
		//【メモ】
		//未完了ファイルと分かるような名前付けて完了後リネームor
		//一時保存先に保存してファイル移動した方がいい
		#define BUF_TEST 256//テスト用、サイズ確定したらbuf使う
		char buf2[BUF_TEST];
		//buf2のサイズとSleepの値を上手く設定しないとダウンロード途中なのにrecvの戻り値が小さくなる？？
    	while (1) {
    		memset(buf2, 0, sizeof(buf2));
    		n = recv(sock, buf2, sizeof(buf2), 0);
    		if (n < 0) {
    			std::cout << "recv : " << WSAGetLastError() << std::endl;
				f.close();
				//deleteFile(savepath);
				closesocket(sock);
				WSACleanup();
    			return -10;
    		}
    		
			Sleep(6);//msec、なぜこれが無いと上手くいかないのか？
    		f.write(buf2, n);
			if (getCancelFlag() == 1) {
				break;
			}
    		if (n < BUF_TEST) {//n == 0だとフリーズする、受信終了判定がイマイチよく分からない
				std::cout << n << std::endl;
    			break;//受信するデータ量が少なかったら最後
    		}
			setNowSize(getNowSize() + BUF_TEST);//pNowSize += BUF_SIZE;
    	}
		setNowPer(2.0);
    	f.close();
		if (getCancelFlag() == 1){
			deleteFile(savepath);
		}
    	closesocket(sock);//ソケットを閉じる
    	WSACleanup();//winsock終了

    	return 1;
    }

	int DirExist(std::string path){
		return PathIsDirectory(path.c_str());
	}

	int makeDirs(std::string path){
		LPTSTR lp = NULL;
		char *fragment;
		std::vector<std::string> words;
		std::string tmp_dir;
		int words_size;

		if (DirExist(path)) {
			std::cout << "DIR EXIST" << std::endl;//既にある
			return 0;
		}

		if (CreateDirectory(path.c_str(), NULL)) {
			std::cout << "CREATE DIR" << std::endl;//普通に作れる
			return 1;
		}

		//再帰的なディレクトリ生成
		fragment = strtok_s(const_cast<char*>(path.c_str()), "\\", &lp);
		while(fragment!= NULL){
			words.push_back(fragment);
			fragment = strtok_s(NULL, "\\", &lp);
		}
		tmp_dir = words[0];
		words_size = words.size();
		for (int i = 1; i < words_size; i++) {
			tmp_dir += "\\" + words[i];
			if (DirExist(tmp_dir)) {
				continue;
			}
			if (CreateDirectory(tmp_dir.c_str(), NULL) == 0) {
				//たぶんドライブが存在しないとか書き込み不可な場所とか
				std::cout << "can't create dir : " << tmp_dir << std::endl;
				return -1;
			}
			std::cout << tmp_dir << std::endl;
		}

		return 1;
	}

	int deleteFile(std::string path, std::string filename = "") {
		if (filename != "") {
			path += "\\" + filename;
		}
		return DeleteFile(path.c_str());
	}

	int FileExist(std::string path, std::string filename = "") {
		if (filename != "") {
			path += "\\" + filename;
		}
		return PathFileExists(path.c_str());
	}

	std::string getNonFilename(std::string path, std::string filename){
		std::vector<std::string> words;
		std::string fname_name = "";
		std::string fname_ext;
		std::string fname_new;
		char fname_add[5];
		int fname_num = 1;
		int path_size = path.size();

		//ファイルが存在しなければそのままのファイル名を返す
		if (path.find_last_of("\\") != path_size || path.find_last_of("/") != path_size) {
			//パスの末尾が「\」又は「/」で無い場合は追加
			path += "\\";
		}
		if(FileExist(path + filename) == 0){
			std::cout << "OK. same file don't exists." << std::endl;
			return filename;
		}

		//fnameを"."で分割してwordsに格納
		//ここから------string.findとstring.substr使ってもっとシンプルに書けそう
		LPTSTR lptszNextToken = NULL;
		char *fragment = strtok_s(const_cast<char*>(filename.c_str()), ".", &lptszNextToken);
		while(fragment!= NULL){
			words.push_back(fragment);
			fragment = strtok_s(NULL, ".", &lptszNextToken);
		}

		int c = words.size();//要素数
		for(int k = 0; k < c-1; k++){
			fname_name += words[k];//文字列として連結(拡張子は除く)
		}
		fname_ext = words[c-1];//最後の要素(拡張子)
		//ここまで------
		while(1){
			//ファイル名が重複してるので(1)の様に末尾に数字を付加
			sprintf_s(fname_add, "(%d).", fname_num++);
			fname_new = fname_name + fname_add + fname_ext;
			if(FileExist(path + fname_new) == 0){
				break;
			}
		}
		return fname_new;//重複しないファイル名を返す
	}

	std::string getDesktop(){
		//デスクトップパスを取得
		std::string ret;
		char Path[256];
		SHGetSpecialFolderPath(NULL, Path, CSIDL_DESKTOP, FALSE);
		ret = Path;
		return ret;
	}

	std::string selectDir() {
		BROWSEINFO bi;
		char folder[260];
		LPITEMIDLIST idl;
		LPMALLOC g_pMalloc;
		SHGetMalloc(&g_pMalloc);

		//ツリータイプ
		//【メモ】名前を付けて保存のも欲しい
		memset(&bi, 0, sizeof(BROWSEINFO));
		bi.hwndOwner = NULL;
		bi.pszDisplayName = folder;
		bi.lpszTitle = TEXT("フォルダを選択してください");
		bi.ulFlags = BIF_RETURNONLYFSDIRS;
		idl = SHBrowseForFolder(&bi);
		if(idl){
			int b = SHGetPathFromIDList(idl, folder);
			if(b){
				return (std::string)folder;
			}
		}
		g_pMalloc->Free(idl);
		return "";
	}

	//以下、プロパティの取得と設定
	float getNowPer(){
		if (pNowPer > 1.0) {
			return 2.0;//完了
		}
		if (pFileSize == -1.0) {
			return -1.0;//不明
		}
		return (float)pNowSize / pFileSize;//進捗状況
	}

	int getNowSize(){
		return pNowSize;
	}

	int getFileSize(){
		return pFileSize;
	}

	int getCancelFlag(){
		return pCancelFlag;
	}

	void setNowPer(float value){
		pNowPer = value;
	}

	void setNowSize(int value){
		pNowSize = value;
	}

	void setFileSize(int value){
		pFileSize = value;
	}

	void setCancelFlag(int value){
		pCancelFlag = value;
	}
private:
	float pNowPer;
	int pNowSize;
	int pFileSize;
	int pCancelFlag;
};
