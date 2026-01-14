# binomial-heap-scheduling-simulator
binomial heap scheduling simulator (final odevi)
Projenin Amacı

Bu program, işlem zamanlama (CPU Scheduling) problemini simüle etmek için yazılmıştır.
Amaç; farklı quantum (q) değerleri için süreçlerin ortalama bekleme süresini (AWT) hesaplamak ve karşılaştırmaktır.

Nasıl Çalışır?

Girdi olarak bir dosyadan:
Process ID
Çalışma süresi (burst time)
Varış zamanı
bilgilerini okur.
Süreçler, Binomial Heap (öncelik kuyruğu) kullanılarak yönetilir.

Her sürecin önceliği:

Kalan çalışma süresi
Sisteme kaçıncı kez girdiği
En uzun işlem süresi (e_max)
dikkate alınarak dinamik olarak hesaplanır.

Zamanlama mantığı:

Her süreç en fazla q (quantum) kadar çalışır
Bitmeyen süreçler küçük bir gecikmeyle tekrar kuyruğa alınır
En yüksek önceliğe sahip süreç her zaman önce çalıştırılır

Program: 

q = 1 → 10 arasındaki tüm quantum değerleri için
Ortalama bekleme süresini hesaplar
Sonuçları tablo halinde ekrana yazdırır
Bu sayede hangi quantum değerinin daha verimli olduğunu gözlemlemek mümkün olur.
