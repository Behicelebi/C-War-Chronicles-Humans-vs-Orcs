#define WIN32_LEAN_AND_MEAN  // (PlaySound çakışması)
#define NOGDI                // GDI (Graphics Device Interface) kısmını devre dışı bırakır (Rectangle çakışması)
#define NOUSER               // User kısmını devre dışı bırakır (CloseWindow, ShowCursor çakışması)
//Bu definelar programın çalışması için oldukça önemli. cURL ve Raylib bir arada kullanılırken oluşan çakışmalar bunlar sayesinde gideriliyor.
#include <windows.h>
#include <winsock2.h>
#include <raylib.h>
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
//Bu program cURL ve Raylib external libraryleri ile çalışmaktadır. Lütfen çalıştırmadan önce kütüphane kurulumlarının doğru yapıldığından emin olunuz !
#define MAX_STRING_LENGTH 100  // Tamamen rastgele bir değer olarak atanmıştır. Daha fazlası kullanılabilir fakat azaltılmasını tavsiye etmiyorum.
#define MAX_STRUCT_LENGTH 12   // "unit_types" Jsonunun max boyutuna göre diğer structlarada atanmış olan değer. Azaltmanız durumundan program düzgün çalışmaz.
#define FILES_DIR_PATH "Files\\"                    //Lütfen doğru klasörü path olarak belirttiğinizden emin olunuz !
#define TEXTURES_DIR_PATH "Files\\Textures\\"       //Lütfen doğru klasörü path olarak belirttiğinizden emin olunuz !
#define SOUNDS_DIR_PATH "Files\\Sounds\\"       //Lütfen doğru klasörü path olarak belirttiğinizden emin olunuz !
#define SCENARIO_LINK "https://yapbenzet.org.tr/"   //Lütfen doğru senaryo linkini belirttiğinizden emin olunuz !
//Bilgilendirmeler genel olarak bu kadar. Kodun ilerleyen kısımlarında önemli yerlerde bilgilendirici yazılar eklenmiştir.

struct Research{
    char isim[MAX_STRING_LENGTH];
    struct Seviye{
        char seviye_isim[MAX_STRING_LENGTH];
        int seviye_num;
        int seviye_deger;
        char seviye_aciklama[MAX_STRING_LENGTH];
        char etkiledigi_birim[MAX_STRING_LENGTH];
        char etkiledigi_deger[MAX_STRING_LENGTH];
    };
    struct Seviye seviye[MAX_STRUCT_LENGTH];
};

struct Unit_Types{
    char irk[MAX_STRING_LENGTH];
    char isim[MAX_STRING_LENGTH];
    float saldiri;
    float savunma;
    int saglik;
    float kritik_sans;
    Color color;
    Texture2D texture_2d;
    Rectangle texture[MAX_STRING_LENGTH];
};

struct Creatures{
    char irk[MAX_STRING_LENGTH];
    char isim[MAX_STRING_LENGTH];
    int etki_degeri;
    char etki_turu[MAX_STRING_LENGTH];
    char etkiledigi_birim[MAX_STRING_LENGTH];
    char aciklama[MAX_STRING_LENGTH];
    Rectangle texture;
    Texture2D texture_2d;
};

struct Heroes{
    char irk[MAX_STRING_LENGTH];
    char isim[MAX_STRING_LENGTH];
    int bonus_degeri;
    char bonus_turu[MAX_STRING_LENGTH];
    char etkiledigi_birim[MAX_STRING_LENGTH];
    char aciklama[MAX_STRING_LENGTH];
    Rectangle texture;
    Texture2D texture_2d;
};

struct Scenario{
    char irk[MAX_STRING_LENGTH];
    char heroNames[MAX_STRUCT_LENGTH][MAX_STRING_LENGTH];
    char creatureNames[MAX_STRUCT_LENGTH][MAX_STRING_LENGTH];
    struct Scenario_Components{
        char name[MAX_STRING_LENGTH];
        int value;
    };
    struct Scenario_Components units[MAX_STRUCT_LENGTH];
    struct Scenario_Components research[MAX_STRUCT_LENGTH];
};

struct Statistics{
    int insan_guc;
    int insan_savunma;
    int insan_net_hasar;
    int ork_guc;
    int ork_savunma;
    int ork_net_hasar;
};

struct UnitDefense {
    char name[MAX_STRING_LENGTH];
    int defense;
};

void getMainJsonFile(int a);
char *EtkiledigiDeger(char X[MAX_STRING_LENGTH]);
char *EtkiledigiBirim(char X[MAX_STRING_LENGTH]);
unsigned long long writeToJson(void*, unsigned int, unsigned int, FILE*);

struct Creatures getCreatures(struct Creatures creature[MAX_STRUCT_LENGTH]);
struct Heroes getHeroes(struct Heroes hero[MAX_STRUCT_LENGTH]);
struct Unit_Types getUnit(struct Unit_Types unit[MAX_STRUCT_LENGTH]);
struct Research getResearch(struct Research research[MAX_STRUCT_LENGTH]);
struct Scenario getScenario(struct Scenario scenario[MAX_STRUCT_LENGTH]);

void addBonuses(struct Creatures creature[MAX_STRUCT_LENGTH],struct Heroes hero[MAX_STRUCT_LENGTH],struct Unit_Types unit[MAX_STRUCT_LENGTH],struct Research research[MAX_STRUCT_LENGTH],struct Scenario scenario[MAX_STRUCT_LENGTH]);
int doCalculations(struct Unit_Types unit[MAX_STRUCT_LENGTH],struct Scenario scenario[MAX_STRUCT_LENGTH],struct Statistics *stats,int,int);
void doVisualization(struct Creatures creature[MAX_STRUCT_LENGTH],struct Heroes hero[MAX_STRUCT_LENGTH],struct Unit_Types unit[MAX_STRUCT_LENGTH],struct Research research[MAX_STRUCT_LENGTH],struct Scenario scenario[MAX_STRUCT_LENGTH]);

int main(){
    clock_t start = clock();

    //Structları tanımlamak
    struct Creatures creature[MAX_STRUCT_LENGTH];
    struct Heroes hero[MAX_STRUCT_LENGTH];
    struct Unit_Types unit_type[MAX_STRUCT_LENGTH];
    struct Research research[MAX_STRUCT_LENGTH];
    struct Scenario scenario[MAX_STRUCT_LENGTH];

    //Structları sıfırlamak
    memset(creature, 0, sizeof(struct Creatures)*MAX_STRUCT_LENGTH);
    memset(hero, 0, sizeof(struct Heroes)*MAX_STRUCT_LENGTH);
    memset(unit_type, 0, sizeof(struct Unit_Types)*MAX_STRUCT_LENGTH);
    memset(research, 0, sizeof(struct Research)*MAX_STRUCT_LENGTH);
    memset(scenario, 0, sizeof(struct Scenario)*MAX_STRUCT_LENGTH);

    //JSON'ları okumak
    *creature = getCreatures(creature); //Adres üzerinden (Struct dizisi olduğundun dolayı)
    *hero = getHeroes(hero);
    *unit_type = getUnit(unit_type);
    *research = getResearch(research);

    //Etkilediği degerleri ve birimleri yazmak
    for(int i=0; i<MAX_STRUCT_LENGTH; i++){
        for(int j=0; j<4; j++){
            strcpy(research[i].seviye[j].etkiledigi_deger, EtkiledigiDeger(research[i].seviye[j].seviye_aciklama));
            strcpy(research[i].seviye[j].etkiledigi_birim, EtkiledigiBirim(research[i].seviye[j].seviye_aciklama));
        }
        strcpy(creature[i].etkiledigi_birim, EtkiledigiBirim(creature[i].aciklama));
        strcpy(hero[i].etkiledigi_birim, EtkiledigiBirim(hero[i].aciklama));
    }

    doVisualization(creature,hero,unit_type,research,scenario);

    clock_t stop = clock();
    double elapsed = (double) (stop - start) / CLOCKS_PER_SEC;
    printf("\n\nGecen sure: %.5fsn\n", elapsed);
    return 0;
}

unsigned long long writeToJson(void *data, unsigned int data_size, unsigned int num_items, FILE *main_scenario) {
    unsigned int content = fwrite(data, data_size, num_items, main_scenario);
    return content;
    //Niye unsigned long long ? Çünkü dosya üzerindeki büyük boyutlu değişikliklerde taşma olmaması için.
    //Bunun yerine t_size da kullanılabilirdi fakat anlaşılabilirliği artırmak adına bunu tercih ettim. (t_size ile unsigned long long aynı şey)
}

void getMainJsonFile(int a) {
    char jsonIndex[2];
    char url[MAX_STRING_LENGTH] = {0};
    sprintf(jsonIndex, "%d.json", a);
    strcpy(url, SCENARIO_LINK);
    strcat(url, jsonIndex);

    char filePath[MAX_STRING_LENGTH];
    sprintf(filePath, "%s%s", FILES_DIR_PATH, "main_scenario.json");
    FILE *main_scenario = fopen(filePath, "wb");

    CURL *curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeToJson);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, main_scenario);
        curl_easy_perform(curl);

        fclose(main_scenario); // Dosya kapatılıyor
        curl_easy_cleanup(curl); // CURL temizleniyor
    }
    fclose(main_scenario);
    sprintf(filePath, "%s%s", FILES_DIR_PATH, "savas_sim.txt");
    FILE *results_file = fopen(filePath, "w");
    fprintf(results_file, "---------------------------------------------------------------\nMain senaryonun alindigi link: %s\n---------------------------------------------------------------\n", url);
    fprintf(results_file, "\n-------------------MAIN SCENARIO %02d LOG FILE-------------------\n\n", a);
    fclose(results_file);
}

char *EtkiledigiDeger(char X[MAX_STRING_LENGTH]){
    if(strstr(X,"savunma")){return "savunma";}
    if(strstr(X,"saldiri")){return "saldiri";}
    if(strstr(X,"kritik vurus sansi")){return "kritik_sans";}
    return "";
}

char *EtkiledigiBirim(char X[MAX_STRING_LENGTH]){
    if(strstr(X,"piyadeler")){return "piyadeler";}
    if(strstr(X,"suvariler")){return "suvariler";}
    if(strstr(X,"okcular")){return "okcular";}
    if(strstr(X,"troller") || strstr(X,"Troller")){return "troller";}
    if(strstr(X,"varg_binicileri")){return "varg_binicileri";}
    if(strstr(X,"ork_dovusculeri")){return "ork_dovusculeri";}
    if(strstr(X,"mizrakcilar")){return "mizrakcilar";}
    if(strstr(X,"kusatma_makineleri")){return "kusatma_makineleri";}
    if(strstr(X,"tum_birimler")){return "tum_birimler";}
    return "";
}

struct Creatures getCreatures(struct Creatures creature[MAX_STRUCT_LENGTH]){
    int index = 0;
    char reading;
    int category = 0;
    char filePath[MAX_STRING_LENGTH];
    sprintf(filePath, "%s%s", FILES_DIR_PATH, "creatures.json");
    FILE *creatures_file = fopen(filePath, "r");
    if (creatures_file == NULL) {
        perror("Dosya 'Files/creatures.json' bulunamadi. Lutfen dosyanin dogru isimde ve dogru dizinde oldugundan emin olun.");
        exit(1);
    }
    while((reading = fgetc(creatures_file)) != EOF) {
        char tempirk[MAX_STRING_LENGTH];
        char tempisim[MAX_STRING_LENGTH];
        if(reading == '{')
            category++;
        else if(reading == '}')
            category--;
        if(category == 2)
            index++;
        char str[2];
        str[1] = '\0';
        while (category == 1)
        {
            if(reading == '"'){
                reading = fgetc(creatures_file); // " Karakterinin atlanmasını sağlar.
                str[0]=reading;
                strcpy(tempirk,str); // Stringe ilk karakteri ekler
                reading = fgetc(creatures_file); // Döngüye girmeden önce karakteri atlar.
                while(reading != '"'){
                    str[0]=reading;
                    strcat(tempirk,str); // Stringe karakter eklemeye devam eder.
                    reading = fgetc(creatures_file); // " Karakterinin atlanmasını sağlar.
                }
            }
            reading = fgetc(creatures_file); //If'e girmediği zaman karakter atlar.
            if(reading == '{')
                category++;
            else if(reading == '}')
                category--;
        }
        while (category == 2){
            if(reading == '"'){
                reading = fgetc(creatures_file);
                str[0]=reading;
                strcpy(tempisim,str);
                reading = fgetc(creatures_file);
                while(reading != '"'){
                    str[0]=reading;
                    strcat(tempisim,str);
                    reading = fgetc(creatures_file);
                }
            }
            reading = fgetc(creatures_file);
            if(reading == '{')
                category++;
            else if(reading == '}')
                category--;
        }
        int flag=0;
        while(category == 3){
            char temp_ozellik[MAX_STRING_LENGTH];
            if(reading == '"'){
                reading = fgetc(creatures_file);
                str[0]=reading;
                strcpy(temp_ozellik,str);
                reading = fgetc(creatures_file);
                while(reading != '"'){
                    str[0]=reading;
                    strcat(temp_ozellik,str);
                    reading = fgetc(creatures_file);
                }
                strcpy(creature[index].irk, tempirk);
                strcpy(creature[index].isim, tempisim);
                if(flag == 1){
                    creature[index].etki_degeri = atoi(temp_ozellik);
                    flag = 0;
                }
                else if(flag == 2){
                    strcpy(creature[index].etki_turu,temp_ozellik);
                    flag = 0;
                }
                else if(flag == 3){
                    strcpy(creature[index].aciklama,temp_ozellik);
                    flag = 0;
                }
                if(strcmp(temp_ozellik,"etki_degeri")==0){
                    flag = 1;
                }
                else if(strcmp(temp_ozellik,"etki_turu")==0){
                    flag = 2;
                }
                else if(strcmp(temp_ozellik,"aciklama")==0){
                    flag = 3;
                }
            }
            reading = fgetc(creatures_file);
            if(reading == '{')
                category++;
            else if(reading == '}')
                category--;
        }
    }
    fclose(creatures_file);
    return *creature;
}

struct Heroes getHeroes(struct Heroes hero[MAX_STRUCT_LENGTH]){
    int index = 0;
    char reading;
    int category = 0;
    char filePath[MAX_STRING_LENGTH];
    sprintf(filePath, "%s%s", FILES_DIR_PATH, "heroes.json");
    FILE *heroes_file = fopen(filePath, "r");
    if (heroes_file == NULL) {
        perror("Dosya 'Files/heroes.json' bulunamadi. Lutfen dosyanin dogru isimde ve dogru dizinde oldugundan emin olun.");
        exit(1);
    }
    while((reading = fgetc(heroes_file)) != EOF) {
        char tempirk[MAX_STRING_LENGTH];
        char tempisim[MAX_STRING_LENGTH];
        if(reading == '{')
            category++;
        else if(reading == '}')
            category--;
        if(category == 2)
            index++;
        char str[2];
        str[1] = '\0';
        while (category == 1)
        {
            if(reading == '"'){
                reading = fgetc(heroes_file); // " Karakterinin atlanmasını sağlar.
                str[0]=reading;
                strcpy(tempirk,str); // Stringe ilk karakteri ekler
                reading = fgetc(heroes_file); // Döngüye girmeden önce karakteri atlar.
                while(reading != '"'){
                    str[0]=reading;
                    strcat(tempirk,str); // Stringe karakter eklemeye devam eder.
                    reading = fgetc(heroes_file); // " Karakterinin atlanmasını sağlar.
                }
            }
            reading = fgetc(heroes_file); //If'e girmediği zaman karakter atlar.
            if(reading == '{')
                category++;
            else if(reading == '}')
                category--;
        }
        while (category == 2){
            if(reading == '"'){
                reading = fgetc(heroes_file);
                str[0]=reading;
                strcpy(tempisim,str);
                reading = fgetc(heroes_file);
                while(reading != '"'){
                    str[0]=reading;
                    strcat(tempisim,str);
                    reading = fgetc(heroes_file);
                }
            }
            reading = fgetc(heroes_file);
            if(reading == '{')
                category++;
            else if(reading == '}')
                category--;
        }
        int flag=0;
        while(category == 3){
            char temp_ozellik[MAX_STRING_LENGTH];
            if(reading == '"'){
                reading = fgetc(heroes_file);
                str[0]=reading;
                strcpy(temp_ozellik,str);
                reading = fgetc(heroes_file);
                while(reading != '"'){
                    str[0]=reading;
                    strcat(temp_ozellik,str);
                    reading = fgetc(heroes_file);
                }
                strcpy(hero[index].irk, tempirk);
                strcpy(hero[index].isim, tempisim);
                if(flag == 1){
                    hero[index].bonus_degeri = atoi(temp_ozellik);
                    flag = 0;
                }
                else if(flag == 2){
                    strcpy(hero[index].bonus_turu,temp_ozellik);
                    flag = 0;
                }
                else if(flag == 3){
                    strcpy(hero[index].aciklama,temp_ozellik);
                    flag = 0;
                }
                if(strcmp(temp_ozellik,"bonus_degeri")==0){
                    flag = 1;
                }
                else if(strcmp(temp_ozellik,"bonus_turu")==0){
                    flag = 2;
                }
                else if(strcmp(temp_ozellik,"aciklama")==0){
                    flag = 3;
                }
            }
            reading = fgetc(heroes_file);
            if(reading == '{')
                category++;
            else if(reading == '}')
                category--;
        }
    }
    fclose(heroes_file);
    return *hero;
}

struct Unit_Types getUnit(struct Unit_Types unit[MAX_STRUCT_LENGTH]){
    int index = 0;
    char reading;
    int category = 0;
    char filePath[MAX_STRING_LENGTH];
    sprintf(filePath, "%s%s", FILES_DIR_PATH, "unit_types.json");
    FILE *unit_file = fopen(filePath, "r");
    if (unit_file == NULL) {
        perror("Dosya 'Files/unit_types.json' bulunamadi. Lutfen dosyanin dogru isimde ve dogru dizinde oldugundan emin olun.");
        exit(1);
    }
    while((reading = fgetc(unit_file)) != EOF) {
        char tempirk[MAX_STRING_LENGTH];
        char tempisim[MAX_STRING_LENGTH];
        if(reading == '{')
            category++;
        else if(reading == '}')
            category--;
        if(category == 2)
            index++;
        char str[2];
        str[1] = '\0';
        while (category == 1)
        {
            if(reading == '"'){
                reading = fgetc(unit_file); // " Karakterinin atlanmasını sağlar.
                str[0]=reading;
                strcpy(tempirk,str); // Stringe ilk karakteri ekler
                reading = fgetc(unit_file); // Döngüye girmeden önce karakteri atlar.
                while(reading != '"'){
                    str[0]=reading;
                    strcat(tempirk,str); // Stringe karakter eklemeye devam eder.
                    reading = fgetc(unit_file); // " Karakterinin atlanmasını sağlar.
                }
            }
            reading = fgetc(unit_file); //If'e girmediği zaman karakter atlar.
            if(reading == '{')
                category++;
            else if(reading == '}')
                category--;
        }
        while (category == 2){
            if(reading == '"'){
                reading = fgetc(unit_file);
                str[0]=reading;
                strcpy(tempisim,str);
                reading = fgetc(unit_file);
                while(reading != '"'){
                    str[0]=reading;
                    strcat(tempisim,str);
                    reading = fgetc(unit_file);
                }
            }
            reading = fgetc(unit_file);
            if(reading == '{')
                category++;
            else if(reading == '}')
                category--;
        }
        int flag=0;
        while(category == 3){
            char temp_ozellik[MAX_STRING_LENGTH];
            if(reading == '"'){
                reading = fgetc(unit_file);
                str[0]=reading;
                strcpy(temp_ozellik,str);
                reading = fgetc(unit_file);
                while(reading != '"'){
                    str[0]=reading;
                    strcat(temp_ozellik,str);
                    reading = fgetc(unit_file);
                }
                strcpy(unit[index].irk, tempirk);
                strcpy(unit[index].isim, tempisim);

                if(strcmp(temp_ozellik,"saldiri")==0){
                    flag = 1;
                }
                else if(strcmp(temp_ozellik,"savunma")==0){
                    flag = 2;
                }
                else if(strcmp(temp_ozellik,"saglik")==0){
                    flag = 3;
                }
                else if(strcmp(temp_ozellik,"kritik_sans")==0){
                    flag = 4;
                }
            }
            else if(reading == ':' && flag!=0){
                reading = fgetc(unit_file);
                reading = fgetc(unit_file);
                str[0]=reading;
                strcpy(temp_ozellik,str);
                reading = fgetc(unit_file);
                while(isdigit(reading) == 1){
                    str[0]=reading;
                    strcat(temp_ozellik,str);
                    reading = fgetc(unit_file);
                }
                if(flag == 1){
                    unit[index].saldiri = atoi(temp_ozellik);
                    flag = 0;
                }
                else if(flag == 2){
                    unit[index].savunma = atoi(temp_ozellik);
                    flag = 0;
                }
                else if(flag == 3){
                    unit[index].saglik = atoi(temp_ozellik);
                    flag = 0;
                }
                else if(flag == 4){
                    unit[index].kritik_sans = atoi(temp_ozellik);
                    flag = 0;
                }
            }
            reading = fgetc(unit_file);
            if(reading == '{')
                category++;
            else if(reading == '}')
                category--;
        }
    }
    fclose(unit_file);
    return *unit;
}

struct Research getResearch(struct Research research[MAX_STRUCT_LENGTH]){
    int index = -1, index1 = 0;
    char reading;
    int category = 0;
    char filePath[MAX_STRING_LENGTH];
    sprintf(filePath, "%s%s", FILES_DIR_PATH, "research.json");
    FILE *research_file = fopen(filePath, "r");
    if (research_file == NULL) {
        perror("Dosya 'Files/research.json' bulunamadi. Lutfen dosyanin dogru isimde ve dogru dizinde oldugundan emin olun.");
        exit(1);
    }
    while((reading = fgetc(research_file)) != EOF) {
        char tempisim1[MAX_STRING_LENGTH];
        char tempseviye[MAX_STRING_LENGTH];
        if(reading == '{')
            category++;
        else if(reading == '}')
            category--;
        if (category == 2){
            index1++;
        }
        if(category == 1){
            index++;
            index1=0;
        }
        char str[2];
        str[1] = '\0';
        while (category == 1)
        {
            if(reading == '"'){
                reading = fgetc(research_file); // " Karakterinin atlanmasını sağlar.
                str[0]=reading;
                strcpy(tempisim1,str); // Stringe ilk karakteri ekler
                reading = fgetc(research_file); // Döngüye girmeden önce karakteri atlar.
                while(reading != '"'){
                    str[0]=reading;
                    strcat(tempisim1,str); // Stringe karakter eklemeye devam eder.
                    reading = fgetc(research_file); // " Karakterinin atlanmasını sağlar.
                }
            }
            reading = fgetc(research_file); //If'e girmediği zaman karakter atlar.
            if(reading == '{')
                category++;
            else if(reading == '}')
                category--;
        }
        while (category == 2){
            if(reading == '"'){
                reading = fgetc(research_file);
                str[0]=reading;
                strcpy(tempseviye,str);
                reading = fgetc(research_file);
                while(reading != '"'){
                    str[0]=reading;
                    strcat(tempseviye,str);
                    reading = fgetc(research_file);
                }
            }
            reading = fgetc(research_file);
            if(reading == '{')
                category++;
            else if(reading == '}')
                category--;
        }
        int flag=0;
        while(category == 3){
            char temp_ozellik[MAX_STRING_LENGTH];
            if(reading == '"'){
                reading = fgetc(research_file);
                str[0]=reading;
                strcpy(temp_ozellik,str);
                reading = fgetc(research_file);
                while(reading != '"'){
                    str[0]=reading;
                    strcat(temp_ozellik,str);
                    reading = fgetc(research_file);
                }
                strcpy(research[index].isim, tempisim1);
                strcpy(research[index].seviye[index1].seviye_isim,tempseviye);
                if(strstr(research[index].seviye[index1].seviye_isim,"seviye_1")){research[index].seviye[index1].seviye_num = 1;}
                else if(strstr(research[index].seviye[index1].seviye_isim,"seviye_2")){research[index].seviye[index1].seviye_num = 2;}
                else if(strstr(research[index].seviye[index1].seviye_isim,"seviye_3")){research[index].seviye[index1].seviye_num = 3;}
                if(flag == 1){
                    research[index].seviye[index1].seviye_deger = atoi(temp_ozellik);
                    flag = 0;
                }
                else if(flag == 2){
                    strcpy(research[index].seviye[index1].seviye_aciklama,temp_ozellik);
                    flag = 0;
                }
                if(strcmp(temp_ozellik,"deger")==0){
                    flag = 1;
                }
                else if(strcmp(temp_ozellik,"aciklama")==0){
                    flag = 2;
                }
            }
            reading = fgetc(research_file);
            if(reading == '{')
                category++;
            else if(reading == '}')
                category--;
        }
    }
    fclose(research_file);
    return *research;
}

struct Scenario getScenario(struct Scenario scenario[MAX_STRUCT_LENGTH]){
    char reading;
    int category = 0,index=-1,index1 = 0,index2 = 0,index3 = 0,index4 = 0;
    char filePath[MAX_STRING_LENGTH];
    sprintf(filePath, "%s%s", FILES_DIR_PATH, "main_scenario.json");
    FILE *scenario_file = fopen(filePath, "r");
    if (scenario_file == NULL) {
        perror("main_scenario.json bulunamadi. Lutfen dosyanin dogru isimde ve dogru dizinde oldugundan emin olun.");
        exit(1);
    }
    while((reading = fgetc(scenario_file)) != EOF){
        int token = 0;
        char tempIrk[MAX_STRING_LENGTH];
        char tempCat[MAX_STRING_LENGTH];
        if(reading == '{' || reading == '[')
            category++;
        else if(reading == '}' || reading == ']')
            category--;
        if(category == 1){
            index++;
            index1 = 0;
            index2 = 0;
            index3 = 0;
            index4 = 0;
        }
        char str[2];
        str[1] = '\0';
        while(category == 1){
            if(reading == '"'){
                reading = fgetc(scenario_file);
                str[0]=reading;
                strcpy(tempIrk,str);
                reading = fgetc(scenario_file);
                while(reading != '"'){
                    str[0]=reading;
                    strcat(tempIrk,str);
                    reading = fgetc(scenario_file);
                }
            }
            reading = fgetc(scenario_file);
            if(reading == '{')
                category++;
            else if(reading == '}')
                category--;
        }
        while (category == 2){
            if(reading == '"'){
                reading = fgetc(scenario_file);
                str[0]=reading;
                strcpy(tempCat,str);
                reading = fgetc(scenario_file);
                while(reading != '"'){
                    str[0]=reading;
                    strcat(tempCat,str);
                    reading = fgetc(scenario_file);
                }
                if(strcmp(tempCat, "kahramanlar") == 0)
                    token = 1;
                else if(strcmp(tempCat, "canavarlar") == 0)
                    token = 2;
                else if(strcmp(tempCat, "birimler") == 0)
                    token = 3;
                else if(strcmp(tempCat, "arastirma_seviyesi") == 0)
                    token = 4;
                else
                    token = 0;
            }
            reading = fgetc(scenario_file);
            if(reading == '{' || reading == '[')
                category++;
            else if(reading == '}' || reading == ']')
                category--;
        }
        while (category == 3){
            if(reading == '"'){
                char tempReading[MAX_STRING_LENGTH];
                reading = fgetc(scenario_file);
                str[0]=reading;
                strcpy(tempReading,str);
                reading = fgetc(scenario_file);
                while(reading != '"'){
                    str[0]=reading;
                    strcat(tempReading,str);
                    reading = fgetc(scenario_file);
                }
                strcpy(scenario[index].irk, tempIrk);
                if(token == 3 || token == 4){
                    if(token==3){
                        strcpy(scenario[index].units[index3].name, tempReading);
                    }
                    else{
                        strcpy(scenario[index].research[index4].name, tempReading);
                    }
                    reading = fgetc(scenario_file);
                    reading = fgetc(scenario_file);
                    str[0] = reading;
                    strcpy(tempReading,str);
                    reading = fgetc(scenario_file);
                    while(isdigit(reading) == 1){
                        str[0]=reading;
                        strcat(tempReading,str);
                        reading = fgetc(scenario_file);
                    }
                    if(token==3){
                        scenario[index].units[index3].value=atoi(tempReading);
                        index3++;
                    }
                    else{
                        scenario[index].research[index4].value=atoi(tempReading);
                        index4++;
                    }
                }
                else if(token == 1 || token == 2){
                    if(token==1){
                        strcpy(scenario[index].heroNames[index1], tempReading);
                        index1++;
                    }
                    else{
                        strcpy(scenario[index].creatureNames[index2], tempReading);
                        index2++;
                    }
                }
            }
            reading = fgetc(scenario_file);
            if(reading == '{' || reading == '[')
                category++;
            else if(reading == '}' || reading == ']')
                category--;
        }
    }
    fclose(scenario_file);
    return *scenario;
}

void addBonuses(struct Creatures creature[MAX_STRUCT_LENGTH],struct Heroes hero[MAX_STRUCT_LENGTH],struct Unit_Types unit[MAX_STRUCT_LENGTH],struct Research research[MAX_STRUCT_LENGTH],struct Scenario scenario[MAX_STRUCT_LENGTH]) {
    for(int j=0; j<2; j++) {
        for(int i=0; i<MAX_STRUCT_LENGTH; i++){
            //Hero Bonuslarını Ayarlamak
            if(strstr(hero[i].isim,scenario[j].heroNames[0]) && strcmp(scenario[j].heroNames[0],"") != 0){
                for(int k=0; k<MAX_STRUCT_LENGTH; k++){
                    if(strcmp(unit[k].irk,scenario[j].irk) == 0 && (strstr(unit[k].isim,hero[i].etkiledigi_birim) || strstr(hero[i].etkiledigi_birim,"tum_birimler"))){
                        if(strstr(hero[i].bonus_turu,"saldiri")){
                            unit[k].saldiri = unit[k].saldiri * (1+(float)hero[i].bonus_degeri/100);
                        }
                        else if(strstr(hero[i].bonus_turu,"savunma")){
                            unit[k].savunma = unit[k].savunma * (1+(float)hero[i].bonus_degeri/100);
                        }
                        else if(strstr(hero[i].bonus_turu,"kritik_sans")){
                            unit[k].kritik_sans = unit[k].kritik_sans * (1+(float)hero[i].bonus_degeri/100);
                        }
                    }
                }
            }
            //Creature Bonuslarını Ayarlamak
            if(strstr(creature[i].isim,scenario[j].creatureNames[0]) && strcmp(scenario[j].creatureNames[0],"") != 0){
                for(int k=0; k<MAX_STRUCT_LENGTH; k++){
                    if(strcmp(unit[k].irk,scenario[j].irk) == 0 && (strstr(unit[k].isim,creature[i].etkiledigi_birim) || strstr(creature[i].etkiledigi_birim,"tum_birimler"))){
                        if(strstr(creature[i].etki_turu,"saldiri")){
                            unit[k].saldiri = unit[k].saldiri * (1+(float)creature[i].etki_degeri/100);
                        }
                        else if(strstr(creature[i].etki_turu,"savunma")){
                            unit[k].savunma = unit[k].savunma * (1+(float)creature[i].etki_degeri/100);
                        }
                        else if(strstr(creature[i].etki_turu,"kritik_sans")){
                            unit[k].kritik_sans = unit[k].kritik_sans * (1+(float)creature[i].etki_degeri/100);
                        }
                    }
                }
            }
            //Research Bonuslarını Ayarlamak
            if(strstr(research[i].isim,scenario[j].research[0].name) && strcmp(scenario[j].research[0].name,"") != 0){
                int indexSeviye=0;
                for(int l=0; l<MAX_STRUCT_LENGTH; l++) {
                    if(research[i].seviye[l].seviye_num == scenario[j].research[0].value){indexSeviye=l;break;}
                }
                for(int k=0; k<MAX_STRUCT_LENGTH; k++){
                    if(strcmp(unit[k].irk,scenario[j].irk) == 0 && (strstr(unit[k].isim,research[i].seviye[indexSeviye].etkiledigi_birim) || strstr(research[i].seviye[indexSeviye].etkiledigi_birim,"tum_birimler"))){
                        if(strstr(research[i].seviye[indexSeviye].etkiledigi_deger,"saldiri")){
                            unit[k].saldiri = unit[k].saldiri * (1+(float)research[i].seviye[indexSeviye].seviye_deger/100);
                        }
                        else if(strstr(research[i].seviye[indexSeviye].etkiledigi_deger,"savunma")){
                            unit[k].savunma = unit[k].savunma * (1+(float)research[i].seviye[indexSeviye].seviye_deger/100);
                        }
                        else if(strstr(research[i].seviye[indexSeviye].etkiledigi_deger,"kritik_sans")){
                            unit[k].kritik_sans = unit[k].kritik_sans * (1+(float)research[i].seviye[indexSeviye].seviye_deger/100);
                        }
                    }
                }
            }
        }
    }
}

int doCalculations(struct Unit_Types unit[MAX_STRUCT_LENGTH], struct Scenario scenario[MAX_STRUCT_LENGTH], struct Statistics *stats, int tur, int yontem){
    struct UnitDefense defStats[MAX_STRUCT_LENGTH];
    char filePath[MAX_STRING_LENGTH];
    sprintf(filePath, "%s%s", FILES_DIR_PATH, "savas_sim.txt");
    FILE *results_file = fopen(filePath, "a");
    int defIndex = 0;
    int kritIndex = 0;
    int krit_liste[MAX_STRUCT_LENGTH] = {0}; // Maksimum 100 ataması yapacak olmak çok fazla geleceğinden 12 ataması yapılmıştır.
    memset(defStats,0,sizeof(struct UnitDefense)*MAX_STRUCT_LENGTH);
    memset(stats,0,sizeof(struct Statistics));

    // Toplan, tekil güçlerin ve savunmaların hesaplanması
    for(int j=0; j<2; j++) {
        for(int i=0; i<MAX_STRUCT_LENGTH; i++){
            for(int k=0; k<MAX_STRUCT_LENGTH; k++){
                if(strstr(scenario[j].units[i].name,unit[k].isim)){
                    if(strcmp(scenario[j].irk,"insan_imparatorlugu") == 0 || strcmp(scenario[j].irk,"ork_legi") == 0){
                        double kritik_vurus = 1;
                        if(unit[k].kritik_sans > 0 && tur % (int)(100/unit[k].kritik_sans * 2 - 1) == 0 && scenario[j].units[i].value != 0 && strcmp(scenario[j].irk,"insan_imparatorlugu") == 0)  {
                            krit_liste[kritIndex] = k;
                            kritik_vurus = 1.5;
                            kritIndex++;
                        }
                        if(unit[k].kritik_sans > 0 && tur % (int)(100/unit[k].kritik_sans * 2) == 0 && scenario[j].units[i].value != 0 && strcmp(scenario[j].irk,"ork_legi") == 0) {
                            krit_liste[kritIndex] = k;
                            kritik_vurus = 1.5;
                            kritIndex++;
                        }
                        if(tur % 10 == 1 && tur != 1) {
                            unit[k].saldiri = unit[k].saldiri * 0.9;
                            unit[k].savunma = unit[k].savunma * 0.9;
                        }
                        if(strcmp(scenario[j].irk,"insan_imparatorlugu") == 0) {
                            stats->insan_guc += scenario[j].units[i].value * unit[k].saldiri * kritik_vurus;
                            stats->insan_savunma += scenario[j].units[i].value * unit[k].savunma;
                        }
                        if(strcmp(scenario[j].irk,"ork_legi") == 0) {
                            stats->ork_guc += scenario[j].units[i].value * unit[k].saldiri * kritik_vurus;
                            stats->ork_savunma += scenario[j].units[i].value * unit[k].savunma;
                        }
                        if(scenario[j].units[i].value * unit[k].savunma) {
                            defStats[defIndex].defense = scenario[j].units[i].value * unit[k].savunma;
                            strcpy(defStats[defIndex].name,scenario[j].units[i].name);
                            defIndex++;
                        }
                    }
                }
            }
        }
    }

    stats->insan_net_hasar = stats->insan_guc * (1 - (float)stats->ork_savunma / stats->insan_guc);
    stats->ork_net_hasar = stats->ork_guc * (1 - (float)stats->insan_savunma / stats->ork_guc);
    if(stats->insan_savunma > stats->ork_guc)
        stats->ork_net_hasar = 0;
    if(stats->ork_savunma > stats->insan_guc)
        stats->insan_net_hasar = 0;

    //Durum bilgilendirmelerinin log file içerisine yazılması.
    if(stats->insan_guc == 0 || stats->insan_savunma == 0) {
        fprintf(results_file, "---------------------------------------------------------------\nORKLAR KAZANDI !\n---------------------------------------------------------------");
        return 2;
    }
    if (stats->ork_guc == 0 || stats->ork_savunma == 0) {
        fprintf(results_file, "---------------------------------------------------------------\nINSANLAR KAZANDI !\n---------------------------------------------------------------");
        return 1;
    }
    if(tur % 2 == 1) {
        fprintf(results_file, "---------------------------------------------------------------\nTUR %d\n", (int)ceil((float)tur/2));
        fprintf(results_file, "\nINSANLAR SALDIRIYOR !!\n\n");
        fprintf(results_file, "Insanların verdigi net hasar: %d\n", stats->insan_net_hasar);
    }else {
        fprintf(results_file, "\nORKLAR SALDIRIYOR !!\n\n");
        fprintf(results_file, "Orkların verdigi net hasar: %d\n", stats->ork_net_hasar);
    }
    if(tur % 10 == 1 && tur != 1)
        fprintf(results_file, "\nBIRLIKLERE YORGUNLUK ETKI EDIYOR !!\n");
    fprintf(results_file, "\nInsan guc: %d\nInsan savunma: %d\n\nOrk guc: %d\nOrk savunma: %d\n", stats->insan_guc,stats->insan_savunma,stats->ork_guc,stats->ork_savunma);
    for(int i = 0; i <= kritIndex; i++) {
        if(krit_liste[i] != 0)
            fprintf(results_file, "\n%s Isimli birim KRITIK VURUS atti.\n\n", unit[krit_liste[i]].isim);
    }
    if(stats->insan_net_hasar == 0 && tur % 2 == 1) {
        fprintf(results_file, "\nINSANLAR ORKLARIN SAVUNMASINI DELMEYI BASARAMADI !\n");
        return 0;
    }
    if(stats->ork_net_hasar == 0 && tur % 2 == 0) {
        fprintf(results_file, "\nORKLAR INSANLARIN SAVUNMASINI DELMEYI BASARAMADI !\n");
        return 0;
    }

    if(yontem == 2) { //Birlik sayılarının azaltılması (Yöntem 1 - Yok etme yöntemi)
        for(int j=0; j<2; j++) {
            for(int i = 0; i < MAX_STRUCT_LENGTH; i++) {
                for(int k = 0; k < MAX_STRUCT_LENGTH; k++) {
                    for(int l = 0; l < MAX_STRUCT_LENGTH; l++) {
                        if(strcmp(scenario[j].units[i].name, defStats[k].name) == 0 && strcmp(scenario[j].irk, "insan_imparatorlugu") == 0 && strcmp(defStats[k].name,"") != 0 && strcmp(scenario[j].units[i].name, unit[l].isim) == 0 && tur % 2 == 0) {
                            int azalma = (int)(stats->ork_net_hasar * ((float)defStats[k].defense / stats->insan_savunma)) / unit[l].saglik;
                            scenario[j].units[i].value -= azalma;
                            if(scenario[j].units[i].value > 0) {
                                fprintf(results_file, "\nSayilari %d olan %s birliğinden %d adeti öldürüldü.\n", scenario[j].units[i].value+azalma, unit[l].isim, azalma);
                            }else { // Fazlalık hasar hesaplaması
                                fprintf(results_file, "\nSayilari %d olan %s birliğinin hepsi yok edildi.", scenario[j].units[i].value+azalma, unit[l].isim);
                                scenario[j].units[i].value = 0;
                            }
                        }
                        if(strcmp(scenario[j].units[i].name, defStats[k].name) == 0 && strcmp(scenario[j].irk, "ork_legi") == 0 && strcmp(defStats[k].name,"") != 0 && strcmp(scenario[j].units[i].name, unit[l].isim) == 0 && tur % 2 == 1) {
                            int azalma = (int)(stats->insan_net_hasar * ((float)defStats[k].defense / stats->ork_savunma)) / unit[l].saglik;
                            scenario[j].units[i].value -= azalma;
                            if(scenario[j].units[i].value > 0) {
                                fprintf(results_file, "\nSayilari %d olan %s birliğinden %d adeti öldürüldü.\n", scenario[j].units[i].value+azalma, unit[l].isim, azalma);
                            }else { // Fazlalık hasar hesaplaması.
                                fprintf(results_file, "\nSayilari %d olan %s birliğinin hepsi yok edildi.\n", scenario[j].units[i].value+azalma, unit[l].isim);
                                scenario[j].units[i].value = 0;
                            }
                        }
                    }
                }
            }
        }
    }
    else if(yontem == 1) { //Birlik sayılarının azaltılması (Yöntem 2 - Can azaltma yöntemi)
        for(int j=0; j<2; j++) {
            for(int i = 0; i < MAX_STRUCT_LENGTH; i++) {
                for(int k = 0; k < MAX_STRUCT_LENGTH; k++) {
                    for(int l = 0; l < MAX_STRUCT_LENGTH; l++) {
                        if(strcmp(scenario[j].units[i].name, defStats[k].name) == 0 && strcmp(scenario[j].irk, "insan_imparatorlugu") == 0 && strcmp(defStats[k].name,"") != 0 && strcmp(scenario[j].units[i].name, unit[l].isim) == 0 && tur % 2 == 0) {
                            int azalma = (int)(stats->ork_net_hasar * ((float)defStats[k].defense / stats->insan_savunma)) / scenario[j].units[i].value;
                            unit[l].saglik -= azalma;
                            if(unit[l].saglik > 0) {
                                fprintf(results_file, "\n%d Canı olan %s birimi %d hasar yiyerek canı %d e düştü.\n", unit[l].saglik+azalma, unit[l].isim, azalma, unit[l].saglik);
                            }else { // Fazlalık hasar hesaplaması.
                                fprintf(results_file, "\n%d Canı olan %s birimi %d hasar yiyerek yok edildi.\n",unit[l].saglik+azalma, unit[l].isim, azalma);
                                scenario[j].units[i].value = 0;
                            }
                        }
                        if(strcmp(scenario[j].units[i].name, defStats[k].name) == 0 && strcmp(scenario[j].irk, "ork_legi") == 0 && strcmp(defStats[k].name,"") != 0 && strcmp(scenario[j].units[i].name, unit[l].isim) == 0 && tur % 2 == 1) {
                            int azalma = (int)(stats->insan_net_hasar * ((float)defStats[k].defense / stats->ork_savunma)) / scenario[j].units[i].value;
                            unit[l].saglik -= azalma;
                            if(unit[l].saglik > 0) {
                                fprintf(results_file, "\n%d Canı olan %s birimi %d hasar yiyerek canı %d e düştü.\n", unit[l].saglik+azalma, unit[l].isim, azalma, unit[l].saglik);
                            }else { // Fazlalık hasar hesaplaması.
                                fprintf(results_file, "\n%d Canı olan %s birimi %d hasar yiyerek yok edildi.\n",unit[l].saglik+azalma, unit[l].isim, azalma);
                                scenario[j].units[i].value = 0;
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
    fclose(results_file);
}

void doVisualization(struct Creatures creature[MAX_STRUCT_LENGTH],struct Heroes hero[MAX_STRUCT_LENGTH],struct Unit_Types unit[MAX_STRUCT_LENGTH],struct Research research[MAX_STRUCT_LENGTH],struct Scenario scenario[MAX_STRUCT_LENGTH]) {
    typedef enum GameScreen { SELECT = 0, GAMEPLAY} GameScreen;
    struct Statistics stats;
    const int screenWidth = 1500;
    const int screenHeight = 960;
    InitWindow(screenWidth, screenHeight, "Humans VS Orcs");
    InitAudioDevice();
    char filePath[MAX_STRING_LENGTH];

    //Sounds
    sprintf(filePath, "%s%s", SOUNDS_DIR_PATH, "war.mp3");
    Sound war = LoadSound(filePath);
    sprintf(filePath, "%s%s", SOUNDS_DIR_PATH, "select.wav");
    Sound select_audio = LoadSound(filePath);
    sprintf(filePath, "%s%s", SOUNDS_DIR_PATH, "next.wav");
    Sound next_audio = LoadSound(filePath);
    sprintf(filePath, "%s%s", SOUNDS_DIR_PATH, "lobby.mp3");
    Sound lobby = LoadSound(filePath);
    sprintf(filePath, "%s%s", SOUNDS_DIR_PATH, "human.mp3");
    Sound human = LoadSound(filePath);
    sprintf(filePath, "%s%s", SOUNDS_DIR_PATH, "orc.mp3");
    Sound orc = LoadSound(filePath);

    //Textures
    sprintf(filePath, "%s%s", TEXTURES_DIR_PATH, "background1.png");
    Texture2D background = LoadTexture(filePath);
    sprintf(filePath, "%s%s", TEXTURES_DIR_PATH, "tile.png");
    Texture2D tile = LoadTexture(filePath);
    sprintf(filePath, "%s%s", TEXTURES_DIR_PATH, "glow_effect.png");
    Texture2D glow = LoadTexture(filePath);
    sprintf(filePath, "%s%s", TEXTURES_DIR_PATH, "glow_effect_debuff.png");
    Texture2D debuff_glow = LoadTexture(filePath);
    sprintf(filePath, "%s%s", TEXTURES_DIR_PATH, "dead.png");
    Texture2D dead= LoadTexture(filePath);
    sprintf(filePath, "%s%s", TEXTURES_DIR_PATH, "alagard.png");
    Font alagard = LoadFont(filePath);
    GameScreen currentScreen = SELECT;
    SetTargetFPS(60);
    int gridSize = 48;

    //Adding custom colors
    Color whiteFade = (Color){255, 255, 255, 48};
    Color blackFade = (Color){0, 0, 0 , 128};

    //Menu Buttons
    Rectangle buttonArray[10];
    const Rectangle play = { screenWidth / 2 - 53, screenHeight / 2 + 230, 100, 50 };
    const Rectangle start = { screenWidth / 2 + 550, screenHeight / 2 - 45, 125, 50 };
    const Rectangle oldur = { screenWidth / 2 - 180, screenHeight / 2, 120, 50 };
    const Rectangle can = { screenWidth / 2 + 60, screenHeight / 2, 120, 50 };
    bool playPressed = false;
    bool startPressed = false;
    Color playColor = whiteFade;
    Color startColor = whiteFade;
    Color oldurColor = whiteFade;
    Color canColor = LIGHTGRAY;
    int yontem = 1;
    for (int i = 0; i < 10; i++) {
        const Rectangle button = { ((screenWidth-690)/2)+(140*(i%5)), 50*((i+1)/6)+200, 130, 40 };
        buttonArray[i] = button;
    }
    Color buttonColorArray[10] = {LIGHTGRAY,whiteFade,whiteFade,whiteFade,whiteFade,whiteFade,whiteFade,whiteFade,whiteFade,whiteFade};
    int buttonClicked = 1;
    int tur = 1;
    int win = 0;

    //Unit Colors
    for (int i = 0; i < MAX_STRUCT_LENGTH; i++) {
        if(strcmp(unit[i].isim,"")!=0) {
            sprintf(filePath, "%s%s%s", TEXTURES_DIR_PATH, unit[i].isim, ".png");
            unit[i].texture_2d = LoadTexture(filePath);
        }
        if(strcmp(hero[i].isim,"")!=0) {
            sprintf(filePath, "%s%s%s", TEXTURES_DIR_PATH, hero[i].isim, ".png");
            hero[i].texture_2d = LoadTexture(filePath);
        }
        if(strcmp(creature[i].isim,"")!=0) {
            sprintf(filePath, "%s%s%s", TEXTURES_DIR_PATH, creature[i].isim, ".png");
            creature[i].texture_2d = LoadTexture(filePath);
        }
    }

    //Default Values
    struct Unit_Types unit_temp[MAX_STRUCT_LENGTH];
    memcpy(unit_temp, unit, sizeof(struct Unit_Types)*MAX_STRUCT_LENGTH);
    struct Scenario scenario_temp[MAX_STRUCT_LENGTH];
    memcpy(scenario_temp, scenario, sizeof(struct Scenario)*MAX_STRUCT_LENGTH);

    // Main game loop
    while (!WindowShouldClose()){
        // Update
        switch(currentScreen){
            case SELECT:{
                if(IsSoundPlaying(lobby)==false){PlaySound(lobby);}
                const Vector2 mousePoint = GetMousePosition();
                for(int i = 0; i < 10; i++) {
                    if(CheckCollisionPointRec(mousePoint, buttonArray[i]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        PlaySound(select_audio);
                        buttonColorArray[i] = LIGHTGRAY;
                        buttonClicked = i+1;
                        for(int j = 0; j < 10; j++) {
                            if(j!=i){buttonColorArray[j] = whiteFade;}
                        }
                    }
                }
                if (CheckCollisionPointRec(mousePoint, play)){
                    playColor = LIGHTGRAY;
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && playPressed==false) {
                        playPressed = true;
                        getMainJsonFile(buttonClicked);
                        sprintf(filePath, "%s%s", FILES_DIR_PATH, "main_scenario.json");
                        FILE *main_scenario = fopen(filePath, "r");
                        fseek(main_scenario, 0, SEEK_END);
                        int length = ftell(main_scenario);
                        if(length == 0) {
                            perror("Main scenario indirilirken sorun olustu !");
                            exit(1);
                        }
                        *scenario = getScenario(scenario);
                        addBonuses(creature,hero,unit,research,scenario);
                        currentScreen = GAMEPLAY;
                    }
                }
                else{playColor = whiteFade;}
                if (CheckCollisionPointRec(mousePoint, oldur) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    PlaySound(select_audio);
                    oldurColor = LIGHTGRAY;
                    canColor = whiteFade;
                    yontem = 2;
                }
                if (CheckCollisionPointRec(mousePoint, can) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    PlaySound(select_audio);
                    oldurColor = whiteFade;
                    canColor = LIGHTGRAY;
                    yontem = 1;
                }
            } break;
            case GAMEPLAY:{
                if(IsSoundPlaying(lobby)==true){StopSound(lobby);}
                if(IsSoundPlaying(war)==false){PlaySound(war);}
                const Vector2 mousePoint = GetMousePosition();
                if (CheckCollisionPointRec(mousePoint, start) && startPressed==false){
                    startColor = LIGHTGRAY;
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        PlaySound(next_audio);
                        int test = doCalculations(unit,scenario,&stats,tur,yontem);
                        if(test == 1 || test == 2){startPressed = true;win=test;}
                        else if(test == 0){tur++;}
                        for (int i = 0; i < MAX_STRUCT_LENGTH; ++i) {
                            for (int j = 0; j < MAX_STRUCT_LENGTH; ++j) {
                                const Rectangle zero = {0,0,0,0};
                                unit[i].texture[j] = zero;
                            }
                        }
                    }
                }
                else{startColor = whiteFade;}
                int test1 = 0,test2 = 0;
                for (int j = 0; j < 2; j++) {
                    for (int i = 0; i < MAX_STRUCT_LENGTH; i++) {
                        for(int k = 0; k < MAX_STRUCT_LENGTH; k++) {
                            if(strcmp(scenario[j].irk,"insan_imparatorlugu")==0) {
                                if(strcmp(unit[i].isim,scenario[j].units[k].name)==0 && strcmp(scenario[j].units[k].name,"")!=0) {
                                    const Rectangle button = {((test1%20)*gridSize)+(screenWidth-gridSize*20)/2,gridSize*(test1/20),gridSize,gridSize};
                                    unit[i].texture[0] = button;
                                    if(scenario[j].units[k].value%100 == 0){for(int l = 0; l < scenario[j].units[k].value/100; l++) {test1++;}}
                                    else{for(int l = 0; l <= scenario[j].units[k].value/100; l++) {test1++;}}
                                    while(test1%20 != 0){test1++;}
                                    test1++;
                                    while(test1%20 != 0){test1++;}
                                }
                            }
                            else if(strcmp(scenario[j].irk,"ork_legi")==0) {
                                if(strcmp(unit[i].isim,scenario[j].units[k].name)==0 && strcmp(scenario[j].units[k].name,"")!=0) {
                                    const Rectangle button = {((test2%20)*gridSize)+(screenWidth-gridSize*20)/2,(gridSize*19)-gridSize*(test2/20),gridSize,gridSize};
                                    unit[i].texture[0] = button;
                                    if(scenario[j].units[k].value%100 == 0){for(int l = 0; l < scenario[j].units[k].value/100; l++) {test2++;}}
                                    else{for(int l = 0; l <= scenario[j].units[k].value/100; l++) {test2++;}}
                                    while(test2%20 != 0){test2++;}
                                    test2++;
                                    while(test2%20 != 0){test2++;}
                                }
                            }
                        }
                    }
                }
                for (int i = 0; i < MAX_STRUCT_LENGTH; i++) {
                    if(strcmp(hero[i].irk,"insan_imparatorlugu")==0 && strcmp(hero[i].isim,"")!=0) {
                        const Rectangle button = {screenWidth/2 - 730,90,gridSize,gridSize};
                        hero[i].texture = button;
                    }
                    if(strcmp(hero[i].irk,"ork_legi")==0 && strcmp(hero[i].isim,"")!=0) {
                        const Rectangle button = {screenWidth/2 - 730,screenHeight - 400,gridSize,gridSize};
                        hero[i].texture = button;
                    }
                    if(strcmp(creature[i].irk,"insan_imparatorlugu")==0 && strcmp(creature[i].isim,"")!=0) {
                        const Rectangle button = {screenWidth/2 + 500, 90,gridSize,gridSize};
                        creature[i].texture = button;
                    }
                    if(strcmp(creature[i].irk,"ork_legi")==0 && strcmp(creature[i].isim,"")!=0) {
                        const Rectangle button = {screenWidth/2 + 500,screenHeight - 400,gridSize,gridSize};
                        creature[i].texture = button;
                    }
                }
            } break;
            default: break;
        }
        // Draw
        BeginDrawing();
            ClearBackground(BLACK);
            switch(currentScreen){
                case SELECT:{
                    DrawTexture(background, 0, 0, WHITE);
                    DrawTextEx(alagard, "Select a Scenario",(Vector2){(screenWidth/2)-220, 100}, 60, 2, WHITE);
                    for(int i = 0; i < 10; i++) {
                        DrawRectangleRec(buttonArray[i], buttonColorArray[i]);
                        char string[11];
                        sprintf(string, "Scenario %d", i+1);
                        DrawTextEx(alagard, string, (Vector2){buttonArray[i].x + 10, buttonArray[i].y + 10}, 21,2, BLACK);
                    }
                    char string1[11];
                    sprintf(string1, "Scenario %d Selected", buttonClicked);
                    DrawTextEx(alagard, string1, (Vector2){screenWidth / 2 - 136, screenHeight / 2 - 150}, 30, 2,  ORANGE);
                    DrawRectangleRec(play, playColor);
                    DrawTextEx(alagard, "PLAY", (Vector2){play.x + 10, play.y + 10}, 35, 2, BLACK);
                    DrawRectangleRec(oldur,oldurColor);
                    DrawTextEx(alagard, "Select a Calculation Method", (Vector2){screenWidth / 2 - 360, screenHeight / 2 - 85}, 60, 2, WHITE);
                    DrawTextEx(alagard, "KILL", (Vector2){oldur.x + 30, oldur.y +10}, 30, 2, BLACK);
                    DrawRectangleRec(can,canColor);
                    DrawTextEx(alagard, "HEALTH", (Vector2){can.x + 7, can.y + 10}, 30, 2, BLACK);
                    if(yontem==1) {DrawTextEx(alagard, "The Health method is based on the reduction of the total life of the troops.",(Vector2){screenWidth / 2 - 380, screenHeight / 2 + 90}, 20, 2, ORANGE);}
                    else if(yontem==2) {DrawTextEx(alagard, "The Kill method is based on reducing the number of troops one by one.",(Vector2){screenWidth / 2 - 360, screenHeight / 2 + 90}, 20, 2, ORANGE);}
                } break;
                case GAMEPLAY:{
                    const Vector2 mousePoint = GetMousePosition();
                    DrawTexture(background, 0, 0, WHITE);
                    for (int i = 0; i < 20*20; i++) {
                        DrawTexture(tile,gridSize*((i%20))+(screenWidth-gridSize*20)/2,(i/20)*gridSize,WHITE);
                    }
                    if(startPressed==false) {
                        DrawRectangleRec(start, startColor);
                        DrawTextEx(alagard,"NEXT", (Vector2){start.x + 20, start.y + 10}, 33, 2, BLACK);
                        char turText[MAX_STRING_LENGTH];
                        sprintf(turText, "ROUND: %d",(int)ceil((float)tur/2));
                        DrawTextEx(alagard,turText,(Vector2){screenWidth / 2 - 685, screenHeight / 2 - 50},30, 2, WHITE);
                        if(tur % 2 == 1)
                            DrawTextEx(alagard,"Insanlar saldiracak !",(Vector2){screenWidth / 2 - 720, screenHeight / 2 - 10},20, 2, WHITE);
                        else
                            DrawTextEx(alagard,"Orklar saldiracak !",(Vector2){screenWidth / 2 - 710, screenHeight / 2 - 10},20, 2, WHITE);
                    }
                    DrawTextEx(alagard, "HERO:",(Vector2){screenWidth/2 - 730,60}, 23, 2, WHITE);
                    DrawTextEx(alagard, "HERO:",(Vector2){screenWidth/2 - 730,screenHeight-430}, 23, 2, WHITE);
                    DrawTextEx(alagard, "CREATURE:",(Vector2){screenWidth/2 + 500,60}, 23, 2, WHITE);
                    DrawTextEx(alagard, "CREATURE:",(Vector2){screenWidth/2 + 500,screenHeight-430}, 23, 2, WHITE);
                    for (int i = 0; i < MAX_STRUCT_LENGTH; i++) {
                        for (int j = 0; j < 2; j++) {
                            for(int k = 0; k < MAX_STRUCT_LENGTH; k++) {
                                if(strcmp(unit[i].isim,scenario[j].units[k].name)==0 && strcmp(unit[i].isim,"")!=0 && scenario[j].units[k].value != 0) {
                                    int value = scenario[j].units[k].value;
                                    int bosluk = value/100;
                                    if(value % 100 != 0) {bosluk++;}
                                    if(bosluk >= 20){bosluk = 1;}
                                    else{bosluk = 20/bosluk;}
                                    for(int l = 0; l <= scenario[j].units[k].value/100; l++) {
                                        char string1[11];
                                        if(value>100){sprintf(string1, "100");}
                                        else{sprintf(string1, "%d", value);}
                                        Color temp_color = GREEN;
                                        if(100*((float)unit[i].saglik/unit_temp[i].saglik) <= 80 && 100*((float)unit[i].saglik/unit_temp[i].saglik) >= 20){temp_color = YELLOW;}
                                        else if(100*((float)unit[i].saglik/unit_temp[i].saglik) < 20 && 100*((float)unit[i].saglik/unit_temp[i].saglik) >= 0){temp_color = RED;}
                                        if(strcmp(unit[i].irk,"insan_imparatorlugu")==0 && unit[i].saglik>0) {
                                            const Rectangle temp = {unit[i].texture[0].x+((l%20)*gridSize*bosluk)+gridSize*(bosluk/2), unit[i].texture[0].y+((l/20)*gridSize),gridSize,gridSize};
                                            DrawTexture(unit[i].texture_2d, temp.x,temp.y, WHITE);
                                            DrawText(string1, temp.x,temp.y, 5, WHITE); //Birlik sayısı font ayaralaması yapılmadı.
                                            if((int)(100*((unit[i].saldiri-unit_temp[i].saldiri)/unit_temp[i].saldiri)) > 0 || (int)(100*((unit[i].savunma-unit_temp[i].savunma)/unit_temp[i].savunma)) > 0 || unit[i].kritik_sans-unit_temp[i].kritik_sans > 0) {
                                                DrawTexture(glow,temp.x,temp.y+36,WHITE);
                                            }
                                            if((int)(100*((unit[i].saldiri-unit_temp[i].saldiri)/unit_temp[i].saldiri)) < 0 || (int)(100*((unit[i].savunma-unit_temp[i].savunma)/unit_temp[i].savunma)) < 0 || unit[i].kritik_sans-unit_temp[i].kritik_sans < 0) {
                                                DrawTexture(debuff_glow,temp.x,temp.y+36,WHITE);
                                            }
                                            DrawRectangle(temp.x+2,temp.y+38,gridSize-5,gridSize-42,BROWN);
                                            DrawRectangle(temp.x+2,temp.y+38,gridSize*((float)unit[i].saglik/unit_temp[i].saglik)-5,gridSize-42,temp_color);
                                            unit[i].texture[l+1] = temp;
                                        }
                                        else if(strcmp(unit[i].irk,"ork_legi")==0 && unit[i].saglik>0) {
                                            const Rectangle temp = {unit[i].texture[0].x+((l%20)*gridSize*bosluk)+gridSize*(bosluk/2), unit[i].texture[0].y-((l/20)*gridSize),gridSize,gridSize};
                                            DrawTexture(unit[i].texture_2d,temp.x,temp.y, WHITE);
                                            DrawText(string1, temp.x,temp.y, 5, WHITE); //Birlik sayısı font ayaralaması yapılmadı.
                                            if((int)(100*((unit[i].saldiri-unit_temp[i].saldiri)/unit_temp[i].saldiri)) > 0 || (int)(100*((unit[i].savunma-unit_temp[i].savunma)/unit_temp[i].savunma)) > 0 || unit[i].kritik_sans-unit_temp[i].kritik_sans > 0) {
                                                DrawTexture(glow,temp.x,temp.y+36,WHITE);
                                            }
                                            if((int)(100*((unit[i].saldiri-unit_temp[i].saldiri)/unit_temp[i].saldiri)) < 0 || (int)(100*((unit[i].savunma-unit_temp[i].savunma)/unit_temp[i].savunma)) < 0 || unit[i].kritik_sans-unit_temp[i].kritik_sans < 0) {
                                                DrawTexture(debuff_glow,temp.x,temp.y+36,WHITE);
                                            }
                                            DrawRectangle(temp.x+2,temp.y+38,gridSize-5,gridSize-42,BROWN);
                                            DrawRectangle(temp.x+2,temp.y+38,gridSize*((float)unit[i].saglik/unit_temp[i].saglik)-5,gridSize-42,temp_color);
                                            unit[i].texture[l+1] = temp;
                                        }
                                        value -= 100;
                                        if(value<=0){break;}
                                    }
                                }
                                if(strcmp(hero[k].isim,scenario[j].heroNames[0])==0 && strcmp(hero[k].isim,"")!=0) {
                                    DrawTexture(hero[k].texture_2d,hero[k].texture.x,hero[k].texture.y + 30,WHITE);
                                    DrawTextEx(alagard, hero[k].isim, (Vector2){hero[k].texture.x,hero[k].texture.y}, 22, 2, WHITE);
                                }
                                if(strcmp(creature[k].isim,scenario[j].creatureNames[0])==0 && strcmp(creature[k].isim,"")!=0) {
                                    DrawTexture(creature[k].texture_2d,creature[k].texture.x,creature[k].texture.y + 30,WHITE);
                                    DrawTextEx(alagard, creature[k].isim, (Vector2){creature[k].texture.x,creature[k].texture.y}, 22, 2, WHITE);
                                }
                            }
                        }
                    }
                    for (int i = 0; i <= gridSize*20; i += gridSize){
                        DrawLine(i+(screenWidth-gridSize*20)/2, 0, i+(screenWidth-gridSize*20)/2, gridSize*20, DARKBROWN);
                        DrawLine((screenWidth-gridSize*20)/2, i, 20*gridSize+(screenWidth-gridSize*20)/2, i, DARKBROWN);
                    }
                    DrawTextEx(alagard, "Insan Imparatorlugu",(Vector2){15,20}, 25, 2, WHITE);
                    DrawTextEx(alagard, "Ork Legi",(Vector2){15,gridSize*19}, 25, 2, WHITE);
                    if(win==1) {
                        DrawTextEx(alagard, "INSANLAR KAZANDI",(Vector2){(screenWidth/4)+166,(screenHeight/2)-47}, 50, 2, BLACK);StopSound(war);if(IsSoundPlaying(human)==false){PlaySound(human);}
                        DrawTexture(dead,screenWidth/2 - 730,screenHeight - 370,WHITE); DrawTexture(dead,screenWidth/2 + 500,screenHeight - 370,WHITE);
                    }
                    else if(win==2) {
                        DrawTextEx(alagard, "ORKLAR KAZANDI",(Vector2){(screenWidth/4)+188,(screenHeight/2)-47}, 50, 2, BLACK);StopSound(war);if(IsSoundPlaying(orc)==false){PlaySound(orc);}
                        DrawTexture(dead,screenWidth/2 + 500, 120,WHITE); DrawTexture(dead,screenWidth/2 - 730,120,WHITE);
                    }
                    for (int i = 0; i < MAX_STRUCT_LENGTH; i++) {
                        for(int j = 1; j < MAX_STRING_LENGTH+1; j++) {
                            if(CheckCollisionPointRec(mousePoint, unit[i].texture[j])) {
                                char str[100];
                                Rectangle square = {mousePoint.x, mousePoint.y, 280, 160};
                                if(strcmp(unit[i].irk,"insan_imparatorlugu")==0) {square.y = mousePoint.y;}
                                else if(strcmp(unit[i].irk,"ork_legi")==0) {square.y = mousePoint.y-160;}
                                DrawRectangleRec(square, blackFade);
                                DrawTextEx(alagard, unit[i].isim,(Vector2){square.x+10,square.y+10}, 20, 2, WHITE);
                                sprintf(str,"saglik: %d/%d",unit[i].saglik,unit_temp[i].saglik);
                                DrawTextEx(alagard, str, (Vector2){square.x+10,square.y+40},20, 2, WHITE);
                                if((int)(100*((unit[i].saldiri-unit_temp[i].saldiri)/unit_temp[i].saldiri)) > 0) {
                                    sprintf(str,"saldiri: %.2f (+%%%d)",unit[i].saldiri,(int)(100*((unit[i].saldiri-unit_temp[i].saldiri)/unit_temp[i].saldiri)));
                                }else if((int)(100*((unit[i].saldiri-unit_temp[i].saldiri)/unit_temp[i].saldiri)) < 0) {
                                    sprintf(str,"saldiri: %.2f (-%%%d)",unit[i].saldiri,abs(100*((unit[i].saldiri-unit_temp[i].saldiri)/unit_temp[i].saldiri)));
                                }else {
                                    sprintf(str,"saldiri: %.2f",unit[i].saldiri);
                                }
                                DrawTextEx(alagard, str,(Vector2){square.x+10,square.y+70}, 20, 2, WHITE);
                                if((int)(100*((unit[i].savunma-unit_temp[i].savunma)/unit_temp[i].savunma)) > 0) {
                                    sprintf(str,"savunma: %.2f (+%%%d)",unit[i].savunma,(int)(100*((unit[i].savunma-unit_temp[i].savunma)/unit_temp[i].savunma)));
                                }else if((int)(100*((unit[i].savunma-unit_temp[i].savunma)/unit_temp[i].savunma)) < 0) {
                                    sprintf(str,"savunma: %.2f (-%%%d)",unit[i].savunma,abs(100*((unit[i].savunma-unit_temp[i].savunma)/unit_temp[i].savunma)));
                                }else {
                                    sprintf(str,"savunma: %.2f",unit[i].savunma);
                                }
                                DrawTextEx(alagard, str,(Vector2){square.x+10,square.y+100}, 20, 2, WHITE);
                                if((int)(100*((unit[i].kritik_sans-unit_temp[i].kritik_sans)/unit_temp[i].kritik_sans)) > 0) {
                                    sprintf(str,"kritik_sans: %%%.2f (+%%%d)",unit[i].kritik_sans,(int)(100*((unit[i].kritik_sans-unit_temp[i].kritik_sans)/unit_temp[i].kritik_sans)));
                                }else if(unit[i].kritik_sans-unit_temp[i].kritik_sans < 0) {
                                    sprintf(str,"kritik_sans: %%%.2f (-%%%d)",unit[i].kritik_sans,abs(100*((unit[i].kritik_sans-unit_temp[i].kritik_sans)/unit_temp[i].kritik_sans)));
                                }else {
                                    sprintf(str,"kritik_sans: %%%.2f",unit[i].kritik_sans);
                                }
                                DrawTextEx(alagard, str, (Vector2){square.x+10,square.y+130}, 20, 2, WHITE);
                            }
                        }
                    }
                } break;
                default: break;
            }
        EndDrawing();
    }
    UnloadSound(war);
    UnloadSound(select_audio);
    UnloadSound(next_audio);
    UnloadSound(lobby);
    UnloadSound(human);
    UnloadSound(orc);
    UnloadTexture(background);
    UnloadTexture(tile);
    UnloadTexture(glow);
    UnloadTexture(debuff_glow);
    UnloadTexture(dead);
    UnloadFont(alagard);
    for (int i = 0; i < MAX_STRUCT_LENGTH; i++) {
        UnloadTexture(unit[i].texture_2d);
        UnloadTexture(hero[i].texture_2d);
        UnloadTexture(creature[i].texture_2d);
    }
    CloseAudioDevice();
    CloseWindow();
}