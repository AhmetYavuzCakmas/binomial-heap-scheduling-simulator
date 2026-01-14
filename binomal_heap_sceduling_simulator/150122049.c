#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define MAX_BUF 256


typedef struct Process {
    char pid[15];
    double kalan_sure;
    double ilk_sure;
    double varis_zamani;
    double oncelik;
    int giris_sayisi;
    double bekleme_suresi;
} Process;

typedef struct BHNode {
    Process *p_data;
    int derece;
    struct BHNode *cocuk, *ebeveyn, *kardes;
} BHNode;

typedef struct BinomialHeap {
    BHNode *head;
} BinomialHeap;

double oncelik_hesapla(Process *p, double e_max) {
    if (p->giris_sayisi == 1) return p->kalan_sure;

    double ust = pow((2 * p->kalan_sure) / (3 * e_max), 3);
    double c_ei = 1.0 / exp(-ust);

    return c_ei * p->kalan_sure;
}
BHNode *yeni_dugum(Process *p) {
    BHNode *node = (BHNode *)malloc(sizeof(BHNode));
    node->p_data = p;
    node->derece = 0;
    node->cocuk = node->ebeveyn = node->kardes = NULL;
    return node;
}

BinomialHeap *yigin_olustur() {
    BinomialHeap *h = (BinomialHeap *)malloc(sizeof(BinomialHeap));
    h->head = NULL;
    return h;
}

BHNode *yigin_birlestir_sirali(BinomialHeap *h1, BinomialHeap *h2) {
    if (!h1->head) return h2->head;
    if (!h2->head) return h1->head;

    BHNode *yeni_bas, *kuyruk, *it1 = h1->head, *it2 = h2->head;
    if (it1->derece <= it2->derece) { yeni_bas = it1; it1 = it1->kardes; }
    else { yeni_bas = it2; it2 = it2->kardes; }
    kuyruk = yeni_bas;

    while (it1 && it2) {
        if (it1->derece <= it2->derece) { kuyruk->kardes = it1; it1 = it1->kardes; }
        else { kuyruk->kardes = it2; it2 = it2->kardes; }
        kuyruk = kuyruk->kardes;
    }
    kuyruk->kardes = it1 ? it1 : it2;
    return yeni_bas;
}

int oncelik_kiyasla(Process *a, Process *b) {
    if (a->oncelik < b->oncelik) return -1;
    if (a->oncelik > b->oncelik) return 1;
    if (a->varis_zamani < b->varis_zamani) return -1;
    return 1;
}

BHNode *yigin_union(BinomialHeap *h1, BinomialHeap *h2) {
    BHNode *new_head = yigin_birlestir_sirali(h1, h2);
    if (!new_head) return NULL;

    BHNode *prev = NULL, *curr = new_head, *next = curr->kardes;
    while (next) {
        if (curr->derece != next->derece || (next->kardes && next->kardes->derece == curr->derece)) {
            prev = curr; curr = next;
        } else {
            if (oncelik_kiyasla(curr->p_data, next->p_data) <= 0) {
                curr->kardes = next->kardes;
                next->ebeveyn = curr; next->kardes = curr->cocuk;
                curr->cocuk = next; curr->derece++;
            } else {
                if (!prev) new_head = next; else prev->kardes = next;
                curr->ebeveyn = next; curr->kardes = next->cocuk;
                next->cocuk = curr; next->derece++;
                curr = next;
            }
        }
        next = curr->kardes;
    }
    return new_head;
}

void yigina_ekle(BinomialHeap *h, Process *p) {
    BinomialHeap *temp = yigin_olustur();
    temp->head = yeni_dugum(p);
    h->head = yigin_union(h, temp);
    free(temp);
}

BHNode *en_kucugu_cikar(BinomialHeap *h) {
    if (!h->head) return NULL;
    BHNode *min = h->head, *min_prev = NULL, *prev = h->head, *curr = h->head->kardes;

    while (curr) {
        if (oncelik_kiyasla(curr->p_data, min->p_data) < 0) { min = curr; min_prev = prev; }
        prev = curr; curr = curr->kardes;
    }

    if (min_prev) min_prev->kardes = min->kardes;
    else h->head = min->kardes;

    BinomialHeap *temp = yigin_olustur();
    BHNode *c = min->cocuk, *n;
    BHNode *n_head = NULL;
    while (c) {
        n = c->kardes;
        c->kardes = n_head;
        c->ebeveyn = NULL;
        n_head = c;
        c = n;
    }
    temp->head = n_head;
    h->head = yigin_union(h, temp);
    free(temp);
    return min;
}

double simule_et(Process *liste, int n, double q, double e_max) {
    BinomialHeap *bh = yigin_olustur();
    double suanki_zaman = 0, toplam_bekleme = 0;
    int bitenler = 0, siradaki_idx = 0;
    double d = 0.1;

    while (bitenler < n || bh->head != NULL) {
        while (siradaki_idx < n && liste[siradaki_idx].varis_zamani <= suanki_zaman) {
            liste[siradaki_idx].giris_sayisi = 1;
            liste[siradaki_idx].oncelik = oncelik_hesapla(&liste[siradaki_idx], e_max);
            yigina_ekle(bh, &liste[siradaki_idx]);
            siradaki_idx++;
        }

        if (bh->head == NULL) {
            if (siradaki_idx < n) suanki_zaman = liste[siradaki_idx].varis_zamani;
            continue;
        }

        BHNode *min_dugum = en_kucugu_cikar(bh);
        Process *p = min_dugum->p_data;

        p->bekleme_suresi += (suanki_zaman - p->varis_zamani);

        double calisma = (p->kalan_sure > q) ? q : p->kalan_sure;
        suanki_zaman += calisma;
        p->kalan_sure -= calisma;

        if (p->kalan_sure > 0) {
            suanki_zaman += d;
            p->varis_zamani = suanki_zaman;
            p->giris_sayisi++;
            p->oncelik = oncelik_hesapla(p, e_max);
            yigina_ekle(bh, p);
        } else {
            suanki_zaman += d;
            bitenler++;
        }
        free(min_dugum);
    }
    for (int i = 0; i < n; i++) toplam_bekleme += liste[i].bekleme_suresi;
    free(bh);
    return toplam_bekleme / n;
}

int main() {
    char dosya_adi[50];
    printf("Giris dosyasi adini girin (orn: input.txt): ");
    scanf("%s", dosya_adi);

    FILE *fp = fopen(dosya_adi, "r");
    if (!fp) { printf("Dosya acilamadi!\n"); return 1; }

    Process orijinal_liste[100];
    int n = 0;
    double e_max = 0;

    while (fscanf(fp, "%s %lf %lf", orijinal_liste[n].pid, &orijinal_liste[n].kalan_sure, &orijinal_liste[n].varis_zamani) != EOF) {
        orijinal_liste[n].ilk_sure = orijinal_liste[n].kalan_sure;
        orijinal_liste[n].bekleme_suresi = 0;
        if (orijinal_liste[n].kalan_sure > e_max) e_max = orijinal_liste[n].kalan_sure;
        n++;
    }
    fclose(fp);

    printf("\nQuantum (q) | Ortalama Bekleme Suresi (AWT)\n");
    printf("------------|---------------------------\n");
    for (double q = 1.0; q <= 10.0; q += 1.0) {
        Process temp[100];
        for(int i=0; i<n; i++) {
            temp[i] = orijinal_liste[i];
            temp[i].bekleme_suresi = 0;
        }
        double awt = simule_et(temp, n, q, e_max);
        printf("%10.1f | %8.3f\n", q, awt);
    }

    return 0;
}
