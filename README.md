# iLMS
iSort Library Management System via C++ TUI  
A beginner-friendly C++ application for practicing OOP concepts by simulating a library system with book management, borrowing/return tracking, and a text-based user interface.

## Installation
Requires a C++ compiler (g++, MinGW, etc.).

```bash
g++ -o iLMS main.cpp
```
Ensure required files are present:
- borrow_history.txt
- return_history.txt
- txt_files/borrowed_books directory

## Usage
Set the terminal size for best display:
- Windows: mode con: cols=49 lines=20
- Linux/macOS: adjust terminal size accordingly.

Run:.
```powershell
/iLMS  # Linux/macOS
iLMS.exe  # Windows
```
Navigate the menu to create, update, or delete book files, manage transactions, or search records.