# C War Chronicles: Humans vs Orcs

## Açıklama
Bu proje, C dili ile yazılmış bir savaş simülasyonudur. İki farklı ırk (İnsan İmparatorluğu ve Ork Lejyonu) arasında gerçekleşen stratejik savaşları modellemeyi amaçlar. Proje kapsamında, savaş birimlerinin (askerlerin), kahramanların ve canavarların özellikleri; saldırı, savunma, sağlık ve kritik vuruş gibi detaylı parametrelerle gerçekçi bir şekilde simüle edilir. Aynı zamanda, yorgunluk, kahraman bonusları, canavar dezavantajları ve araştırma seviyeleri gibi ek faktörler de savaşın gidişatını etkiler.

## İçindekiler
Genel Bakış
Özellikler
Kullanılan Teknolojiler ve Kütüphaneler
Kurulum ve Çalıştırma
JSON Dosyaları
Senaryoların Dinamik İndirilmesi
Simülasyon Mekaniği ve Savaş Adımları
Görselleştirme (Raylib)
Sonuç Dosyası
Katkıda Bulunanlar
Gelecekteki Geliştirmeler
Kaynakça

## Kullanılan Teknolojiler ve Kütüphaneler
- C Programlama Dili
Projenin ana dili, düşük seviyeli bellek yönetimi ve hız avantajları nedeniyle tercih edilmiştir.

- Struct Yapısı
Birimlerin (asker, kahraman, canavar) özelliklerini saklamak için struct yapılandırmaları kullanılır.

- libcurl
İnternetten JSON senaryo dosyalarının dinamik olarak indirilmesi için kullanılır.

- Raylib
Savaş alanının ve birimlerin grafik arayüzde gerçek zamanlı olarak gösterilmesini sağlar.

- JSON İşleme (Manuel Ayrıştırma)
Hazır kütüphaneler kullanılmadan, dosyalar satır satır okunarak anahtar-değer eşleştirmeleri yapılır.

## Genel Bakış
Bu proje, C dilinin temel özelliklerini (özellikle struct kullanımı) ön plana çıkararak, iki farklı ırk arasındaki savaşları simüle eder. Kullanıcı, internet üzerinden bir senaryo seçip indirdikten sonra simülasyonu başlatır. Simülasyon, gerçek zamanlı olarak aşağıdaki bileşenleri dikkate alır:

Birimlerin (asker, kahraman, canavar vb.) saldırı, savunma ve sağlık parametreleri
Kahramanların ve canavarların getirdiği bonus veya dezavantajlar
Araştırma seviyelerinin birim performansına etkileri
Kritik vuruş ve yorgunluk mekanizmaları
Simülasyon çıktıları, hem bir metin dosyasında (savas_sim.txt) hem de grafik arabirim (Raylib) üzerinden anlık takip edilebilir.

## Özellikler
Detaylı Savaş Mekaniği: Saldırı, savunma, kritik vuruş, yorgunluk ve benzeri tüm değişkenler gerçek zamanlı olarak hesaplanır.
Kahraman & Canavar Etkisi: Kahramanlar, ordularına bonuslar; canavarlar da rakip ordulara dezavantajlar sağlar.
JSON Tabanlı Veri Girişi: Birim, kahraman, canavar ve araştırma verileri JSON formatında tutulur. Program bu verileri okuyarak simülasyonu özelleştirir.
İnternetten Dinamik Senaryo İndirme: libcurl kütüphanesi ile seçilen senaryo otomatik olarak indirilir.
Grafik Arayüz (Raylib): Savaş alanı 20x20’lik bir ızgarada görselleştirilir, birimlerin sağlık barları ve durumları anlık olarak takip edilebilir.
Loglama ve Kayıt: Savaşın her aşaması, gerçekleştirilen tüm hesaplamalarla birlikte savas_sim.txt dosyasına kaydedilir.

## Kurulum ve Çalıştırma
1. Bağımlılıklar
GCC veya Clang gibi bir C derleyicisi
libcurl (internet üzerinden dinamik senaryo dosyası indirmek için)
Raylib (görselleştirme için)
Linux/Unix Tabanlı Sistemlerde (Ubuntu, Debian vb.) bağımlılıkların kurulumu:

bash
Kodu kopyala
### libcurl kurulumu
sudo apt-get update
sudo apt-get install libcurl4-openssl-dev

### Raylib kurulumu (Paket yöneticisinde varsa)
sudo apt-get install libraylib-dev

### Gerekli değilse kendiniz kaynak kodundan da derleyebilirsiniz
### Daha fazla bilgi için: https://github.com/raysan5/raylib
2. Derleme
Proje dosyalarınızı bir klasöre indirdikten sonra, aşağıdaki gibi derleyebilirsiniz:

bash
### Örnek derleme komutu (dosya adlarınızı ve konumlarınızı uyarlayın)
gcc -o savas_sim main.c libcurl_entegrasyon.c raylib_entegrasyon.c json_ayristirma.c -lcurl -lraylib
Not: Parametreler ve dosya isimleri proje yapınıza göre değişebilir.

3. Çalıştırma
Derleme işlemi sorunsuz tamamlandığında:

bash
./savas_sim
Program çalıştığında, sizden bir senaryo seçmeniz veya belirli bir URL girmeniz istenebilir. Seçtiğiniz senaryo libcurl aracılığıyla otomatik olarak indirilecek ve simülasyon başlayacaktır.


## JSON Dosyaları
Proje; birim, kahraman, canavar ve araştırma seviyeleri gibi bilgileri saklamak için 4 temel JSON dosyasını kullanır:

unit_types.json
Birim türlerinin (saldırı, savunma, sağlık, kritik şans vb.) değerleri.
heroes.json
Kahramanların sağladığı bonuslar.
creatures.json
Canavarların sağladığı avantajlar ve rakibe verdikleri dezavantajlar.
research.json
Araştırma seviyelerine bağlı olarak orduların güç artış veya savunma bonusları.
Manuel Ayrıştırma
JSON dosyaları, kütüphane kullanmadan satır satır okunur ve anahtar-değer çiftleri aranır. Okunan değerler, ilgili struct alanlarına atanır. Sırası değişebilecek anahtarlar esnek bir şekilde yönetilir. Eksik veya hatalı veriler için varsayılan değerler atanır.


## Senaryoların Dinamik İndirilmesi
Proje, çalıştırma esnasında bir senaryo URL’si seçerek veya otomatik bir seçim yoluyla (libcurl kullanarak) JSON formatında senaryo dosyasını indirir. Bu dosya, hangi birimlerin hangi sayılarda savaşa katılacağını, hangi kahramanların veya canavarların devrede olduğunu belirtir.

Dosya indirilemezse: Program kullanıcıya bir hata mesajı gösterir ve simülasyon başlamaz.
Dosya indirilebilirse: İlgili senaryo JSON verisi parse edilir ve simülasyona dahil edilir.

## Simülasyon Mekaniği ve Savaş Adımları
Tur Bazlı İşleyiş
Her turda sırasıyla İnsan İmparatorluğu ve Ork Lejyonu birbirine saldırır (hangi tarafın ilk saldıracağı senaryoya göre değişebilir).
Saldırı ve Savunma Hesabı
Her birimin saldırı gücü, birim sayısı ve kahraman bonuslarıyla çarpılır. Karşı tarafın savunma değeriyle karşılaştırılarak net hasar hesaplanır.
Kritik Vuruş
Birimler, belirli bir olasılıkla (JSON’daki kritik_sans) kritik vuruş yapar ve ek hasar uygular.
Yorgunluk
Savaş uzadıkça, her 5 turda bir saldırı ve savunma gücü %10 azalır.
Birim Kaybı
Hasar alan birimlerin sağlık puanları düşer. Sağlığı sıfırın altına inen birimler yok olur.
Kazanan Belirleme
Bir tarafın bütün birimleri yok olduğunda veya senaryoda tanımlı başka bir bitiş şartı oluştuğunda savaş biter.

## Görselleştirme (Raylib)
Savaş Alanı: 20x20’lik bir ızgara olarak modellenir.
Birimler: Farklı resim/simgeler ile temsil edilir. Her bir hücrede maksimum 100 asker bulunabilir.
Sağlık Durumu: Her birimin anlık sağlık durumu, hücre üzerinde gösterilen bir sağlık barı ile kullanıcıya sunulur.
Etkileşim: Fare birimin üzerindeyken kahraman veya canavar bonusları gibi ek bilgiler görüntülenir.


## Sonuç Dosyası
Simülasyon sırasında yapılan her saldırı, savunma, kritik vuruş, kaybedilen asker sayısı ve bonus/dezavantaj bilgileri savas_sim.txt dosyasına kaydedilir. Tur bazında kaydedilen bu veriler, savaşın tüm aşamalarını adım adım analiz etmenize olanak tanır.

Örnek bir bölüm:

yaml
Kodu kopyala
TUR 8
Insan guc: 15030
Insan savunma: 9594
Ork guc: 43860
Ork savunma: 15795
ORKLAR SALDIRIYOR !!
Orkların verdigi net hasar: 35516
varg_binicileri Isimli birim KRITIK VURUS atti.
40 Canı olan kusatma_makineleri birimi 216 hasar yiyerek yok edildi.
...
ORKLAR KAZANDI !


## Katkıda Bulunanlar
Murat Emre Biçici

Savaş mekaniklerinin matematiksel modellemesi, saldırı-savunma hesaplamaları ve libcurl entegrasyonu
Algoritmaların doğrulanması ve fonksiyonların geliştirilmesi

Behiç Çelebi

JSON dosyalarının manuel ayrıştırılması ve senaryo işleme algoritmalarının geliştirilmesi
Raylib kullanarak görselleştirmenin yapılması ve grafik arayüzün tasarlanması

Her iki yazar da projenin genel mimarisini birlikte tasarlamış, test süreçlerinde ve hata yönetiminde ortak çalışmıştır.



## Screenshots
<table>
 <tr>
  <td>demo1</td>
  <td>demo2</td>
  <td>demo3</td>
  <td>demo4</td>
 </tr>
 <tr>
  <td><img src="https://github.com/Behicelebi/School_Project1/blob/main/screenshots/c1.jpg"></td>
  <td><img src="https://github.com/Behicelebi/School_Project1/blob/main/screenshots/c2.jpg"></td>
  <td><img src="https://github.com/Behicelebi/School_Project1/blob/main/screenshots/c3.jpg"></td>
  <td><img src="https://github.com/Behicelebi/School_Project1/blob/main/screenshots/c4.jpg"></td>
 </tr>
</table>
