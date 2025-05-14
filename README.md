# Bubble Shooter (C & Raylib)

Klasik Bubble Shooter oyununun C dili ve Raylib kütüphanesi ile modüler olarak geliştirilmiş sürümüdür.

## Özellikler
- Altıgen grid üzerinde renkli balonlar
- Oyuncu, açı ayarlayarak balon fırlatır
- 3+ aynı renkte balon birleşince patlama
- Tavana bağlı olmayan balonlar düşer
- Puan ve seviye sistemi
- Menü, duraklatma ve oyun sonu ekranları
- Temiz, modüler ve açıklamalı C kodu

## Dosya Yapısı
- `main.c`         : Ana oyun döngüsü
- `game.c/h`       : Oyun mantığı ve yönetimi
- `bubble.c/h`     : Balon nesneleri ve grid
- `player.c/h`     : Oyuncu kontrolü
- `collision.c/h`  : Çarpışma ve patlatma algoritmaları
- `ui.c/h`         : Arayüz ve skor gösterimi
- `utils.c/h`      : Yardımcı fonksiyonlar
- `Makefile`       : Derleme otomasyonu

## Bağımlılıklar
- [Raylib](https://www.raylib.com/) (v4.5+ önerilir)
- C derleyicisi (gcc/clang/MinGW)

### Raylib Kurulumu
- **Linux:** `sudo apt install libraylib-dev`
- **macOS:** `brew install raylib`
- **Windows:** [Raylib Installer](https://github.com/raysan5/raylib/releases) veya MSYS2/MinGW ile kurulum

## Derleme
```sh
make
```
- Windows: `make` veya `mingw32-make`
- Linux/macOS: `make`

## Çalıştırma
```sh
./bubbleshooter    # Linux/macOS
bubbleshooter.exe  # Windows
```

## Notlar
- Kodda bellek sızıntısı yoktur, tüm kaynaklar Raylib tarafından yönetilir.
- Kodun tamamı modüler ve açıklamalıdır.
- Geliştirme ve test: Windows 10, Ubuntu 22.04, macOS Sonoma

## Lisans
MIT 