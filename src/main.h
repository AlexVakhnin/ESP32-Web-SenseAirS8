//Это внешние функции для main.cpp(слово "extern" можно опускать)

void getGraphArr(); //вызывается раз в 5 сек.
void serial2_clear();
void disp_setup();
void ip_show(String mod, IPAddress ip);
void wifi_show(int i);
String readFile(fs::FS &fs, const char * path);
void initSPIFFS();
void web_init();
void handler_2s_job();
void handler_1s_job();