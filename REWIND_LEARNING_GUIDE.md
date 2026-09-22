# Rewind Project - Learning Guide

## 🎯 مقدمه
پروژه Rewind یک emulator frontend multi-platform است برای:
- ✅ Windows
- ✅ macOS  
- ✅ Android
- ✅ iOS

---

## 📚 مراحل یادگیری

### مرحله 1: MAUI یادگیری
**مدت:** 1-2 هفته

#### موارد کلیدی:
1. **XAML** - UI markup language
2. **Data Binding** - ارتباط بین UI و Data
3. **MVVM Pattern** - UI architecture
4. **Navigation** - جابجایی بین صفحات
5. **Platform-Specific Code** - کد مخصوص هر platform

#### منابع:
- Microsoft MAUI Documentation
- MAUI Tutorial (Microsoft Docs)
- Lemuroid Repository (برای مثال)

---

### مرحله 2: معماری‌های مختلف
یکی از این 4 معماری را انتخاب کن:

#### 1️⃣ **MVVM + Shared Services** ⭐ ساده‌ترین
```
تکیه: UI logic separation
سطح پیچیدگی: ⭐⭐
کد reuse: ⭐⭐⭐
Platform Support: ⭐⭐⭐

ساختار:
├─ Core/
│  ├─ Models/
│  ├─ Services/
│  └─ (No UseCase layer)
├─ Desktop/ (MAUI)
├─ Mobile/ (MAUI)
└─ Shared/

مناسب برای: شروع سریع، کم overhead
```

---

#### 2️⃣ **Hexagonal Architecture (Ports & Adapters)** ⭐⭐⭐ توصیه شده
```
تکیه: Business logic independence
سطح پیچیدگی: ⭐⭐⭐
کد reuse: ⭐⭐⭐⭐
Platform Support: ⭐⭐⭐⭐

ساختار:
├─ Core/
│  ├─ Models/
│  ├─ Services/
│  └─ Ports/ (Interfaces)
├─ Adapters/
│  ├─ SqliteAdapter
│  ├─ LibretroAdapter
│  └─ FileSystemAdapter
├─ Desktop/ (MAUI)
├─ Mobile/ (MAUI)
└─ Shared/

مناسب برای: 4 platform، flexibility بالا
```

---

#### 3️⃣ **Clean Architecture** ⭐⭐⭐ محافظه‌کار
```
تکیه: Complete separation of concerns
سطح پیچیدگی: ⭐⭐⭐⭐
کد reuse: ⭐⭐⭐⭐
Platform Support: ⭐⭐⭐

ساختار:
├─ Core/
│  ├─ Domain/
│  │  ├─ Entities/
│  │  └─ UseCases/
│  └─ Data/
│     ├─ Repositories/
│     └─ Database/
├─ Desktop/ (MAUI)
├─ Mobile/ (MAUI)
└─ Shared/

مناسب برای: بزرگ‌تر scaling، strict separation
```

---

#### 4️⃣ **Vertical Slice Architecture** ⭐⭐⭐ متوازن
```
تکیه: Feature-based organization
سطح پیچیدگی: ⭐⭐⭐
کد reuse: ⭐⭐⭐
Platform Support: ⭐⭐⭐

ساختار:
├─ Core/
│  ├─ Games/
│  │  ├─ GetGamesQuery
│  │  ├─ GameService
│  │  └─ GameRepository
│  ├─ SaveStates/
│  ├─ Consoles/
│  └─ Settings/
├─ Desktop/ (MAUI)
├─ Mobile/ (MAUI)
└─ Shared/

مناسب برای: Modular features، independent development
```

---

## 🔧 فناوری‌های استفاده شده

### Database
- **SQLite** - Cross-platform, portable
- **Entity Framework Core** - ORM

### Emulation
- **lrcpp** - Libretro C++ Wrapper
- **Libretro API** - 25 توابع

### UI Framework
- **MAUI** - .NET MAUI (Xaml + C#)
- **Jetpack Compose** (برای Reference: Lemuroid)

### Patterns
- **MVVM** - UI pattern
- **Repository Pattern** - Data access
- **Dependency Injection** - Loose coupling

---

## 📋 Database Schema

```sql
Tables:
├── Consoles
│   ├── id (PK)
│   ├── name
│   └── icon_path
│
├── Games
│   ├── id (PK)
│   ├── console_id (FK)
│   ├── name
│   ├── file_path
│   ├── is_favorite
│   └── last_played
│
├── SaveStates
│   ├── id (PK)
│   ├── game_id (FK)
│   ├── slot (1-5)
│   ├── data
│   └── created_at
│
├── GameMetadata
│   ├── id (PK)
│   ├── game_id (FK)
│   ├── total_playtime
│   ├── first_played
│   ├── rating
│   └── notes
│
└── Settings
    ├── key
    └── value
```

---

## 🎯 انتخاب معماری

### برای Rewind: **Hexagonal + MVVM** توصیه می‌شود

**چرا؟**
- ✅ 4 platform برای cover کردن
- ✅ Code sharing بین platforms
- ✅ Platform-specific adapters سهل
- ✅ Independent core business logic
- ✅ Flexible برای تغییرات

---

## 📖 ترتیب یادگیری

### هفته 1-2: MAUI Basics
```
Day 1-3: XAML + Data Binding
Day 4-5: MVVM + ViewModel
Day 6-7: Navigation + Pages
Day 8-10: Layouts + Controls
Day 11-14: Mini Project (TODO App)
```

### هفته 3: معماری‌های مختلف
```
Day 15: MVVM + Services خوانده
Day 16: Hexagonal خوانده
Day 17: Clean vs Vertical Slice
Day 18: تصمیم‌گیری و شروع
```

### هفته 4+: Rewind Project شروع
```
Day 19-21: Core Setup
Day 22-28: Database + Models
Day 29+: UI + ViewModels
```

---

## 🔗 منابع مفید

### MAUI
- https://learn.microsoft.com/dotnet/maui/
- https://github.com/dotnet/maui

### Libretro
- https://docs.libretro.com/
- https://github.com/libretro/

### Reference Projects
- https://github.com/Swordfish90/Lemuroid (Android)
- https://github.com/Swordfish90/Rewind (Windows/C#)

---

## ✅ Checklist

- [ ] MAUI Basics فهمیدم
- [ ] MVVM رو یاد گرفتم
- [ ] 4 معماری رو خوندم
- [ ] Database Schema فهمیدم
- [ ] Hexagonal + MVVM رو انتخاب کردم
- [ ] اولین MAUI page رو ساختم
- [ ] Repository pattern رو پیاده‌سازی کردم
- [ ] SQLite رو setup کردم
- [ ] Libretro API رو یاد گرفتم
- [ ] اولین emulation فریم رو اجرا کردم

---

## 🚀 Next Steps

1. **MAUI رو یاد بگیر** (هفته 1-2)
2. **معماری‌ها رو بخون** (هفته 3)
3. **Core structure رو ساز** (هفته 4)
4. **Database رو setup کن** (هفته 5)
5. **UI صفحات رو بساز** (هفته 6+)

---

**Good luck! 🎮**
