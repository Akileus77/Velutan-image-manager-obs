## 🎉 Velutan Image Manager v1.0.0 - İlk Stabil Sürüm!

OBS Studio için modern arkaplan ve karakter yönetim eklentisi.

### ✨ Özellikler

- 🎬 **Sahneye Özel Arkaplanlar** - Her sahne kendi arkaplanını bağımsız tutar
- 🖼️ **Aktif Asset Gösterimi** - Aktif arkaplan ve karakterler listenin en üstünde
- 🌍 **Tema Bazlı Organizasyon** - 100+ tema ile arkaplanları kategorize edin
- 🏷️ **Tag Filtreleme** - Arkaplan ve karakterler için ayrı tag filtreleri
- ✏️ **Asset Düzenleme** - İsim, tag ve tema bilgilerini düzenleyin
- 🗑️ **Asset Silme** - İstenmeyen asset'leri kaldırın
- 🎨 **Modern UI** - Dark tema ile profesyonel arayüz
- 📷 **Thumbnail Önizleme** - 80x80px görsel önizlemeler
- 📐 **Auto-Stretch** - Arkaplanları otomatik ekran boyutuna ölç
- 👤 **Sahneye Özel Karakterler** - Her sahnede bağımsız karakter yönetimi

### 📋 Gereksinimler

- OBS Studio 30.0+ (Qt6 sürümü)
- Windows 10/11 (64-bit)

### 📥 Kurulum

#### Otomatik Kurulum (Önerilen) 🚀
1. **`Velutan-Image-Manager-v1.0.0-Installer.exe`** dosyasını indirin
2. İndirilen dosyayı **yönetici olarak çalıştırın** (sağ tık → "Yönetici olarak çalıştır")
3. Kurulum sihirbazını takip edin
4. OBS Studio'yu yeniden başlatın
5. `View → Docks → Velutan Image Manager` ile açın

#### Manuel Kurulum 📦
1. **`Velutan-Image-Manager-v1.0.0-Manual.zip`** dosyasını indirin
2. ZIP'i açın
3. `INSTALL.txt` dosyasındaki talimatları takip edin
4. OBS Studio'yu yeniden başlatın

### 📖 Kullanım

1. OBS'de: `View → Docks → Velutan Image Manager`
2. `Tools → Velutan Image Manager (Setup)` ile asset ekleyin
3. Scene seçin ve arkaplan/karakter ekleyin!

Detaylı kullanım için [README.md](https://github.com/Akileus77/Velutan-image-manager-obs/blob/main/README.md) dosyasına bakın.

### 🔐 Güvenlik Doğrulaması

SHA256 checksum dosyası (`SHA256SUMS.txt`) ile indirdiğiniz dosyaların bütünlüğünü doğrulayabilirsiniz:

```powershell
Get-FileHash Velutan-Image-Manager-v1.0.0-Installer.exe -Algorithm SHA256
```

Çıktı `SHA256SUMS.txt` dosyasındaki hash ile eşleşmelidir.

### 📦 Dosyalar

| Dosya | Boyut | Açıklama |
|-------|-------|----------|
| `Velutan-Image-Manager-v1.0.0-Installer.exe` | 9.53 MB | Otomatik kurulum (önerilen) |
| `Velutan-Image-Manager-v1.0.0-Manual.zip` | 0.08 MB | Manuel kurulum paketi |
| `SHA256SUMS.txt` | < 1 KB | Checksum doğrulama dosyası |

### 🐛 Sorun Giderme

**Plugin görünmüyor:**
- OBS Studio'nun 30.0+ (Qt6) versiyonu olduğundan emin olun
- OBS'yi tamamen kapatıp yeniden başlatın
- `Help → Log Files → View Current Log` ile "[Velutan]" loglarını kontrol edin

**Qt DLL hataları:**
- Installer kullandıysanız, tüm gerekli DLL'ler otomatik yüklenmiştir
- Manuel kurulum yaptıysanız, tüm Qt6*.dll dosyalarını kopyaladığınızdan emin olun

Daha fazla yardım için: [GitHub Issues](https://github.com/Akileus77/Velutan-image-manager-obs/issues)

### 🎬 Video Tutorial

*(Yakında eklenecek)*

### 🙏 Teşekkürler

- OBS Studio ekibine harika yazılım için
- Qt Project'e güçlü UI framework için
- Tüm test kullanıcılarına geri bildirimler için

---

**OBS Studio topluluğu için ❤️ ile yapıldı.**

Eğer bu plugin işinize yaradıysa, ⭐ yıldız vermeyi unutmayın!

