# Release Guide - Velutan Image Manager

Bu rehber, projenizi başkalarıyla paylaşmak için release hazırlama adımlarını açıklar.

## 📋 Önkoşullar

### Gerekli Araçlar

1. **Visual Studio 2019/2022** (zaten kurulu)
2. **CMake** (zaten kurulu)
3. **Qt 6.7.2** (zaten kurulu)
4. **NSIS (Nullsoft Scriptable Install System)** - Sadece installer için
   - İndir: https://nsis.sourceforge.io/Download
   - Kurulum: Varsayılan ayarlarla kurun

### Opsiyonel
- **Git** - GitHub'a yüklemek için
- **GitHub hesabı** - Online paylaşım için

---

## 🚀 Hızlı Başlangıç (Manuel Paket)

En basit yöntem - sadece ZIP dosyası oluşturma:

### Adım 1: Build'i Kontrol Et

```cmd
cd build
cmake --build . --config Release
```

Build başarılı olmalı ve şu dosyalar oluşmalı:
- `build\Release\velutan-image-manager.dll`
- `build\Release\Qt6Core.dll`
- `build\Release\Qt6Gui.dll`
- `build\Release\Qt6Widgets.dll`

### Adım 2: Quick Release Script'ini Çalıştır

```cmd
quick-release.bat
```

Script size versiyon numarası soracak (örn: 1.0.0).

### Adım 3: ZIP Oluştur

`release\v1.0.0\manual\` klasörünü sağ tık → "Send to" → "Compressed (zipped) folder"

### Adım 4: Paylaş

ZIP dosyasını:
- E-posta ile gönderin
- Google Drive/Dropbox'a yükleyin
- Discord/WhatsApp ile paylaşın

---

## 📦 Tam Release Paketi (Manuel + Installer)

Profesyonel dağıtım için hem manuel hem installer paketleri:

### Adım 1: NSIS'i Kur

1. https://nsis.sourceforge.io/Download adresinden indirin
2. Kurulumu çalıştırın (varsayılan ayarlar)
3. Kurulum tamamlandıktan sonra PowerShell'i yeniden başlatın

### Adım 2: PowerShell Script'ini Çalıştır

PowerShell'i **Yönetici olarak** açın:

```powershell
# Execution policy'yi ayarla (ilk kez için)
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser

# Release script'ini çalıştır
.\prepare-release.ps1 -Version "1.0.0"
```

### Parametreler (opsiyonel):
```powershell
.\prepare-release.ps1 `
    -Version "1.0.0" `
    -OBSDir "C:\Program Files\obs-studio" `
    -Qt6Dir "C:\Qt\6.7.2\msvc2019_64"
```

### Script Ne Yapar?

1. ✅ `release\v1.0.0\` klasörü oluşturur
2. ✅ Manuel paket ZIP'i hazırlar
3. ✅ NSIS installer'ı build eder
4. ✅ Dokümantasyon kopyalar
5. ✅ SHA256 checksum'ları hesaplar
6. ✅ Release notes oluşturur

### Çıktı Dosyaları:

```
release\v1.0.0\
├── Velutan-Image-Manager-v1.0.0-Manual.zip
├── Velutan-Image-Manager-v1.0.0-Installer.exe
├── README.md
├── INSTALL.md
├── LICENSE
├── RELEASE_NOTES.md
└── SHA256SUMS.txt
```

---

## 🌐 GitHub'da Release Yayınlama

### Manuel Yöntem

#### 1. GitHub Repository Oluştur

1. https://github.com/new adresine git
2. Repository adı: `Velutan-image-manager-obs`
3. Description: "OBS Studio plugin for managing backgrounds and characters"
4. Public veya Private seç
5. "Create repository" tıkla

#### 2. Kodu GitHub'a Yükle

```bash
# İlk kez için
git init
git add .
git commit -m "Initial commit"
git branch -M main
git remote add origin https://github.com/Akileus77/Velutan-image-manager-obs.git
git push -u origin main
```

#### 3. Release Oluştur

1. GitHub repository'de "Releases" → "Create a new release" tıkla
2. "Choose a tag" → Yeni tag oluştur: `v1.0.0`
3. Release title: `Velutan Image Manager v1.0.0`
4. Description: `RELEASE_NOTES.md` içeriğini kopyala
5. "Attach binaries" → Dosyaları sürükle:
   - `Velutan-Image-Manager-v1.0.0-Manual.zip`
   - `Velutan-Image-Manager-v1.0.0-Installer.exe`
   - `SHA256SUMS.txt`
6. "Publish release" tıkla

### Otomatik Yöntem (GitHub Actions)

Projeye `.github/workflows/release.yml` eklendi. Bu otomatik build yapacak:

#### Kullanım:

```bash
# Tag oluştur ve push et
git tag v1.0.0
git push origin v1.0.0
```

GitHub Actions otomatik olarak:
1. Projeyi build eder
2. Release paketi oluşturur
3. GitHub Release yayınlar

**Not:** İlk kullanımda GitHub Actions için OBS SDK setup'ı gerekebilir.

---

## 📝 Release Checklist

Release yapmadan önce kontrol et:

### Kod Kalitesi
- [ ] Tüm özellikler çalışıyor
- [ ] Bilinen bug'lar düzeltildi
- [ ] OBS'de test edildi (birden fazla sahnede)
- [ ] Build hatası yok

### Dokümantasyon
- [ ] README.md güncel
- [ ] INSTALL.md güncel
- [ ] Version numaraları güncellendi
- [ ] Screenshot'lar eklendi (opsiyonel)

### Paketler
- [ ] Manuel ZIP paketi oluşturuldu
- [ ] Installer (varsa) test edildi
- [ ] Tüm DLL'ler dahil
- [ ] Data klasörü dahil

### Test
- [ ] Temiz bir sistemde manuel kurulum test edildi
- [ ] Installer (varsa) test edildi
- [ ] Plugin OBS'de görünüyor
- [ ] Tüm temel özellikler çalışıyor

---

## 🎯 Versiyon Numaralandırma

Semantic Versioning kullanın: `MAJOR.MINOR.PATCH`

- **MAJOR** (1.0.0): Büyük değişiklikler, uyumsuz API değişiklikleri
- **MINOR** (1.1.0): Yeni özellikler, geriye dönük uyumlu
- **PATCH** (1.0.1): Bug düzeltmeleri

Örnekler:
- `1.0.0` - İlk stabil release
- `1.1.0` - Yeni özellik eklendi
- `1.0.1` - Bug düzeltmesi
- `2.0.0` - Büyük yeniden yapılanma

---

## 📤 Dağıtım Kanalları

### 1. GitHub Releases (Önerilen)
**Artıları:**
- ✅ Ücretsiz hosting
- ✅ Otomatik güncelleme bildirimleri
- ✅ SHA256 checksum desteği
- ✅ İstatistikler

**Nasıl:**
- Yukarıdaki "GitHub'da Release Yayınlama" bölümüne bakın

### 2. OBS Forum
**Nereye:** https://obsproject.com/forum/resources/

**Nasıl:**
1. Forum hesabı oluştur
2. "Resources" → "Submit a Resource"
3. Category: "OBS Studio Plugins"
4. Detayları doldur, dosyaları yükle

### 3. Discord / Reddit
**Topluluklar:**
- r/obs (Reddit)
- OBS Discord sunucusu
- Türkçe yayıncı toplulukları

**Nasıl:**
- README.md'den kısa bir tanıtım yaz
- Screenshot'lar ekle
- GitHub release linkini paylaş

### 4. Kişisel Website/Blog
Basit bir HTML sayfası:
```html
<!DOCTYPE html>
<html>
<head>
    <title>Velutan Image Manager</title>
</head>
<body>
    <h1>Velutan Image Manager for OBS</h1>
    <p>Modern background and character management for OBS Studio</p>
    <a href="release/Velutan-Image-Manager-v1.0.0-Installer.exe">Download Installer</a>
    <a href="release/Velutan-Image-Manager-v1.0.0-Manual.zip">Download Manual</a>
</body>
</html>
```

---

## 🔒 Güvenlik

### Checksum'ları Paylaş

Her zaman SHA256 checksum'larını ekleyin:

```powershell
# Otomatik oluşturulur prepare-release.ps1 ile
# Manuel oluşturmak için:
Get-FileHash *.zip, *.exe -Algorithm SHA256 | Format-Table Hash, Path
```

Kullanıcılar doğrulayabilir:
```powershell
Get-FileHash Velutan-Image-Manager-v1.0.0-Manual.zip -Algorithm SHA256
```

### Code Signing (İleri Seviye)

Installer'ı imzalamak için:
1. Code signing sertifikası alın (ücretli)
2. SignTool ile imzalayın
3. Kullanıcılar Windows SmartScreen uyarısı görmez

---

## 🛠️ Sorun Giderme

### "NSIS bulunamadı" Hatası

**Çözüm:**
1. NSIS'i tekrar kur
2. PATH'e eklendiğinden emin ol
3. PowerShell'i yeniden başlat

### "Qt DLL bulunamadı" Hatası

**Çözüm:**
```powershell
# Qt dizinini belirt
.\prepare-release.ps1 -Qt6Dir "C:\Qt\6.7.2\msvc2019_64"
```

### Build Bulunamadı

**Çözüm:**
```cmd
cd build
cmake --build . --config Release
```

---

## 📊 İstatistikler ve Feedback

### GitHub Release İstatistikleri

GitHub otomatik olarak gösterir:
- İndirme sayıları
- Hangi dosyalar indirildi
- Traffic analytics

### Kullanıcı Feedback'i

Feedback toplamak için:
1. GitHub Issues'ı aktif et
2. Discussions özelliğini aç
3. E-posta adresi ekle README'ye

---

## 🎓 Best Practices

1. **Düzenli Release'ler**
   - Her büyük özellik için minor version
   - Bug fix'ler için patch version

2. **Changelog Tutun**
   - Her değişikliği kaydedin
   - CHANGELOG.md dosyası oluşturun

3. **Geriye Dönük Uyumluluk**
   - Kullanıcı ayarlarını koruyun
   - Breaking change'ler için MAJOR version artırın

4. **Test Edin**
   - Her release'i test edin
   - Temiz bir sistemde deneyin

5. **Dokümante Edin**
   - Kurulum adımlarını netleştirin
   - Screenshot'lar ekleyin
   - Video tutorial hazırlayın (opsiyonel)

---

## 📞 Yardım

Sorularınız için:
- GitHub Issues açın
- E-posta: your-email@example.com
- Discord: YourUsername#1234

---

**Başarılı release'ler! 🚀**

