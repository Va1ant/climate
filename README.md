Контроллер отопительного модуля системы кондиционирования воздуха.<br>
Управляется МК attiny85, который вычисляет скорость изменения температуры, полученной с термометра ds18s20 и принимает решение о замыкании или размыкании силовой цепи через реле.<br>
Между измерениями МК переходит в режим пониженного энергопотребления.<br>
Возникающие ошибки регистрируются и передаются мастер-устройству при подключении.<br>
Взаимодействие с мастер-устройством осуществляется по однопроводному протоколу OneWire.<br>
