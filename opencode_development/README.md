# Maintenance & Repair Management System - Portable Edition

A complete, self-contained maintenance management system that runs on any computer with Python installed. No installation required - just unzip and run!

## 🚀 Quick Start (Super Easy!)

### Windows
1. **Unzip** the downloaded file
2. **Double-click** `install_windows.ps1`
3. **That's it!** The installer handles everything automatically:
   - Downloads and installs Python if needed
   - Verifies system compatibility
   - Opens user guide
   - Launches the application

### Mac
1. **Unzip** the downloaded file
2. **Double-click** `install_mac.applescript`
3. **That's it!** The installer handles everything automatically:
   - Downloads and installs Python if needed
   - Verifies system compatibility
   - Opens user guide
   - Launches the application

### Linux
1. **Unzip** the downloaded file
2. **Open terminal** in the unzipped folder
3. **Run**: `chmod +x setup.sh && ./setup.sh`
4. **Run**: `python3 maintenance_gui.py`

## 📖 What Happens During Installation

The installer automatically:
- ✅ Checks if Python is installed
- ✅ Downloads and installs Python if missing
- ✅ Verifies tkinter is available
- ✅ Opens the user guide with instructions
- ✅ Launches the maintenance application
- ✅ Loads sample data for immediate use

**No technical experience required!**

## 📋 What's Included

### One-Click Installers
- **`install_windows.ps1`** - Windows installer (downloads Python if needed)
- **`install_mac.applescript`** - Mac installer (downloads Python if needed)
- **`setup.sh`** - Linux/Unix setup checker

### Core Applications
- **`maintenance_gui.py`** - Full-featured desktop GUI application
- **`maintenance_manager.py`** - Command-line version
- **`demo_maintenance.py`** - Demo with sample data
- **`super_ai_manager.py`** - Intelligent multi-agent orchestration

### Web/PWA Version (Optional)
- **`maintenance_web_api.py`** - Flask REST API
- **`static/index.html`** - Web interface
- **`static/manifest.json`** - PWA configuration
- **`static/sw.js`** - Service worker for offline use

### Documentation & Guides
- **`USER_GUIDE.md`** - Simple user guide (opens automatically)
- **`README.md`** - Technical documentation
- **`GUI_README.md`** - Detailed GUI documentation
- **`MAINTENANCE_README.md`** - CLI documentation
- **`WEB_PWA_README.md`** - Web version documentation
- **`MOBILE_APP_REQUIREMENTS.md`** - Mobile development guide

### Testing & Development
- **`test_*.py`** - Various test scripts
- **`requirements.txt`** - Python dependencies (for web version)

## 🔧 System Requirements

### Minimum Requirements
- **Python 3.6+** (download from python.org if needed)
- **tkinter** (usually included with Python)
- **SQLite3** (included with Python)
- **200 MB free disk space**

### Supported Operating Systems
- ✅ **Windows 7+** (32-bit and 64-bit)
- ✅ **macOS 10.12+** (Intel and Apple Silicon)
- ✅ **Linux** (Ubuntu, CentOS, Fedora, etc.)
- ✅ **Raspberry Pi** (with desktop environment)

## 📱 Application Features

### Desktop GUI (Primary Interface)
- **Dashboard** - Real-time workload overview
- **Facilities** - Multi-facility management
- **Rooms** - Room-based organization
- **Equipment** - Asset tracking and management
- **Technicians** - Staff management and assignment
- **Work Orders** - Complete maintenance task tracking

### Key Capabilities
- ✅ **Room-Based Work Orders** - Assign tasks to specific locations
- ✅ **Multi-Facility Support** - Manage multiple buildings/sites
- ✅ **Equipment Tracking** - Monitor assets and maintenance schedules
- ✅ **Technician Management** - Staff scheduling and skills tracking
- ✅ **Real-Time Dashboard** - Live workload and status monitoring
- ✅ **Data Export/Import** - Backup and data portability

## 🎯 How to Use

### First Time Setup
1. **Check System**: Run `setup.bat` (Windows) or `setup.sh` (macOS/Linux)
2. **Start Application**: Double-click `maintenance_gui.py` or run `python3 maintenance_gui.py`
3. **Load Sample Data**: The app automatically loads sample facilities, rooms, equipment, and technicians

### Daily Usage
1. **Check Dashboard** - View workload and overdue items
2. **Create Work Orders** - Add maintenance tasks by facility and room
3. **Assign Technicians** - Assign qualified staff to tasks
4. **Track Progress** - Update work order status as tasks complete
5. **Generate Reports** - Export data for analysis

### Data Management
- **Automatic Backup**: Database created locally (`maintenance.db`)
- **Export Data**: File → Export Data (JSON format)
- **Import Data**: File → Import Data (restore from backup)
- **Data Portability**: Move database file between computers

## 🏗️ Architecture

### Database Schema
- **facilities** - Building/site information
- **rooms** - Room/area details within facilities
- **equipment** - Asset inventory and specifications
- **technicians** - Staff information and skills
- **work_orders** - Maintenance tasks and tracking

### File Structure
```
maintenance_portable/
├── maintenance_gui.py          # Main desktop application
├── maintenance_manager.py      # Command-line version
├── maintenance_web_api.py      # Web API (optional)
├── static/                     # Web interface files
├── setup.bat                   # Windows setup
├── setup.sh                    # Unix setup
├── README.md                   # This file
├── *.md                        # Documentation
└── test_*.py                   # Test scripts
```

## 🔒 Security & Privacy

- **Local Data**: All data stored locally on your computer
- **No Internet Required**: Works completely offline
- **No Data Collection**: No telemetry or external communications
- **SQLite Encryption**: Database can be encrypted if needed
- **Access Control**: Single-user application (can be extended)

## 🚨 Troubleshooting

### "Python not found" Error
**Solution**: Download and install Python 3.6+ from python.org
- Windows: Download installer, check "Add to PATH"
- macOS: Use Homebrew (`brew install python3`) or download installer
- Linux: Use package manager (`sudo apt install python3 python3-tk`)

### "tkinter not available" Error
**Solution**: Install tkinter for your system
- Ubuntu/Debian: `sudo apt install python3-tk`
- CentOS/RHEL: `sudo yum install tkinter`
- macOS: Usually included with Python installer
- Windows: Usually included with Python installer

### Application Won't Start
**Solution**:
1. Run setup script to verify system
2. Check Python version (3.6+ required)
3. Try running from command line: `python3 maintenance_gui.py`
4. Check for error messages in terminal/console

### Database Issues
**Solution**:
1. Delete `maintenance.db` file (loses data)
2. Restart application (creates fresh database)
3. Import backup data if available

## 📞 Support

### Self-Help Resources
- **Setup Scripts**: Run `setup.bat` or `setup.sh` for system checks
- **Documentation**: Check included README files
- **Demo**: Run `demo_maintenance.py` to see sample data

### Getting Help
- Check the included documentation files
- Run test scripts to verify functionality
- Review error messages for specific issues

## 🔄 Updates & Versions

### Current Version
- **Desktop GUI**: Complete with room-based management
- **Web PWA**: Optional mobile-friendly interface
- **Database**: SQLite with full schema
- **Compatibility**: Python 3.6+ across all platforms

### Future Updates
- Download new zip file from source
- Overwrite existing files (keep database backup)
- Run setup script to verify compatibility

## 📄 License & Terms

This software is provided as-is for maintenance management purposes. Modify and distribute as needed for your organization's requirements.

## 🙏 Acknowledgments

Built with Python, tkinter, and SQLite for maximum portability and reliability.

---

**Ready to manage your maintenance operations? Just unzip and start!** 🎉</content>
<parameter name="filePath">README.md