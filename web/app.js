let editingAlarmId = null;

// ===== Helper LocalStorage =====
function saveData(key, data) {
    localStorage.setItem(key, JSON.stringify(data));
}

function loadData(key) {
    const data = localStorage.getItem(key);
    return data ? JSON.parse(data) : null;
}

// ===== Modal Control =====
function openModal(modalId) {
    const modal = document.getElementById(modalId);
    modal.classList.add('active');

    const now = new Date();
    if (modalId === 'timeModal') {
        document.getElementById('timeInput').value = now.toTimeString().slice(0, 5);
    } else if (modalId === 'dateModal') {
        document.getElementById('dateInput').value = now.toISOString().slice(0, 10);
    } else if (modalId === 'alarmModal') {
        if (!editingAlarmId) {
            document.getElementById('alarmInput').value = now.toTimeString().slice(0, 5);
            document.getElementById('alarmModalTitle').textContent = 'Tambah Alarm';
        } else {
            document.getElementById('alarmModalTitle').textContent = 'Ubah Alarm';
        }
    }
}

function closeModal(modalId) {
    const modal = document.getElementById(modalId);
    modal.classList.remove('active');
    if (modalId === 'alarmModal') {
        editingAlarmId = null;
    }
}

window.onclick = function (event) {
    if (event.target.classList.contains('modal')) {
        event.target.classList.remove('active');
        if (event.target.id === 'alarmModal') {
            editingAlarmId = null;
        }
    }
};
// ===== Placeholder =====
function saveCustomText() {
    const now = new Date();

    const inputEl = document.getElementById('textInput');
    if (!inputEl) {
        console.error('Elemen input dengan id "textInput" tidak ditemukan.');
        return;
    }

    // Ambil nilai input
    let textValue = inputEl.value.trim();

    // Jika kosong → simpan string kosong
    if (textValue === "") {
        textValue = "";
    }

    const payload = {
        custom_text: textValue,
        updated_at: now.toISOString()
    };

    saveData("text_settings", payload);

    console.log("Custom teks tersimpan:", payload);
    alert("Teks berhasil disimpan!");
}

// ===== Waktu & Tanggal =====
function submitTime(event) {
    event.preventDefault();
    const time = document.getElementById('timeInput').value;

    let timeData = loadData("time_settings") || {};
    timeData.custom_time = time;
    timeData.updated_at = new Date().toISOString();

    saveData("time_settings", timeData);
    closeModal('timeModal');
}

function submitDate(event) {
    event.preventDefault();
    const date = document.getElementById('dateInput').value;

    let timeData = loadData("time_settings") || {};
    timeData.custom_date = date;
    timeData.updated_at = new Date().toISOString();

    saveData("time_settings", timeData);
    closeModal('dateModal');
}

// ===== Alarm =====
function submitAlarm(event) {
    event.preventDefault();
    const time = document.getElementById('alarmInput').value;

    let alarms = loadData("alarms") || [];

    if (editingAlarmId) {
        // update alarm
        alarms = alarms.map(alarm =>
            alarm.id === editingAlarmId ? { ...alarm, alarm_time: time, updated_at: new Date().toISOString() } : alarm
        );
    } else {
        // tambah alarm baru
        const newAlarm = {
            id: Date.now(),
            alarm_time: time,
            is_active: true,
            created_at: new Date().toISOString()
        };
        alarms.push(newAlarm);
    }

    saveData("alarms", alarms);
    editingAlarmId = null;
    closeModal('alarmModal');
    loadAlarms();
}

function loadAlarms() {
    const alarms = loadData("alarms") || [];
    const alarmList = document.getElementById('alarmList');
    alarmList.innerHTML = '';

    if (alarms.length > 0) {
        alarms
            .sort((a, b) => a.alarm_time.localeCompare(b.alarm_time))
            .forEach(alarm => {
                const alarmItem = document.createElement('div');
                alarmItem.className = 'alarm-item';

                const timeButton = document.createElement('button');
                timeButton.className = 'alarm-time';
                timeButton.textContent = alarm.alarm_time.slice(0, 5);
                timeButton.onclick = () => editAlarm(alarm.id, alarm.alarm_time);

                const deleteButton = document.createElement('button');
                deleteButton.className = 'delete-btn';
                deleteButton.innerHTML = '<svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><polyline points="3 6 5 6 21 6"/><path d="M19 6v14a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V6m3 0V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2"/></svg>';
                deleteButton.onclick = () => deleteAlarm(alarm.id);

                alarmItem.appendChild(timeButton);
                alarmItem.appendChild(deleteButton);
                alarmList.appendChild(alarmItem);
            });
    }
}

function editAlarm(alarmId, alarmTime) {
    editingAlarmId = alarmId;
    document.getElementById('alarmInput').value = alarmTime.slice(0, 5);
    openModal('alarmModal');
}

function deleteAlarm(alarmId) {
    let alarms = loadData("alarms") || [];
    alarms = alarms.filter(a => a.id !== alarmId);
    saveData("alarms", alarms);
    loadAlarms();
}

// ===== Tombol Kirim utama =====
function saveDateTime() {
    const now = new Date();
    const customTime = document.getElementById("timeInput").value || now.toTimeString().slice(0, 5);
    const customDate = document.getElementById("dateInput").value || now.toISOString().slice(0, 10);

    const updated = {
        custom_time: customTime,
        custom_date: customDate,
        updated_at: now.toISOString()
    };

    saveData("time_settings", updated);
    console.log("Waktu & tanggal tersimpan:", updated);
    alert("Waktu & tanggal berhasil disimpan!");
}

function saveAlarms() {
    let alarms = loadData("alarms") || [];
    saveData("alarms", alarms);
    console.log("Alarm tersimpan:", alarms);
    alert("Semua alarm berhasil disimpan!");
}

// ===== Init =====
window.addEventListener('DOMContentLoaded', () => {
    updateTemperature();
    updateHumidity();
    loadAlarms();
});
