<div style="text-align: center; margin-top: 150px; margin-bottom: 250px;">
  <p style="font-size: 14px; text-transform: uppercase; letter-spacing: 2px;">Федеральное государственное образовательное бюджетное
учреждение высшего образования
«Сибирский государственный университет телекоммуникаций и
информатики» (ФГОБУ ВО «СибГУТИ»)</p>
  <h1 style="font-size: 36px; margin-top: 40px;">Отчет SDR №1</h1>
  <p style="font-size: 18px; color: gray;">GNU Radio. Построение радио-приёмника.</p>
</div>

<div style="text-align: right; margin-right: 50px;">
  <p><b>Выполнил:</b> Студент / Хоробров Георгий ИКС-433</p>
  <p><b>Проверил:</b> Преподаватель / Калачиков Александр Александрович</p>
</div>

<div style="text-align: center; margin-top: 150px;">
  <p>Новосибирск — 2026</p>
</div>

<div style="page-break-after: always;"></div>

---

### Создали конфигурацию по методичке

Общая схема ![1.png](../images/1.png)

`QT GUI `

> Создали QT GUI Range для задания частоты радио
> ![3.png](../images/3.png)

`PlutoSDR Source`

> Связали с `QT GUI Range` через `id` и привели к типу `int`. Также указали `ip` SDR: 192.168.3.1 или 192.168.2.1
> ![4.png](../images/4.png)

###### **Low Pass Filter** - пропускает частоты ниже 100 кГц и подавляет все что выше

###### **WBFM Receive** - превращает отфильтрованный сигнал и демодулирует его

###### **Audio Sink** - блок вывода звука на звуковую карту компьютера

### Программа

![2.png](../images/2.png)
