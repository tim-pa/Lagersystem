# Lagersystem

Eine Desktopanwendung zur Verwaltung von einem Lagerbestand.

![Hauptansicht](Hauptansicht.png)

## Funktionen

- Suchfunktion
- Ein- /Ausbuchung
- Buchungsverlauf
- Änderungen (Artikellöschung/ Veränderung /Erstellung)
- Änderungsverlauf
- Warnung bei Unterschreitung der Mindestmenge

## Verwendete Technologien

- C++
- Qt (LGPL)
- SQLite
- Visual Studio (Community)

## Build und Ausführung

Projekt mit Visual Studio öffnen, Qt installieren, Anwendung bauen.
Eine Beispieldatenbank wird beim ersten Start automatisch erzeugt.

## Design

**Entwurfsmuster: angelehnt an Model View Controller**
In der "Lagersystem.cpp" werden alle "View-Elemente" mit den Datenmodellen verknüpft, sowie die Benutzereingaben an die Controller-Funktionen der "LogicController.cpp" übergeben. 
Die "Lagersystem.cpp" entspricht dem View Teil und verknüpft die View mit den Datenmodellen. 
Die "LogicController.cpp" entspricht dem Controller Teil. Qt realisiert die Verknüpfung der Datenbank mit dem Modell über ein QSqlQueryModel.

**Warum angelehnt an Model View Controller**
Einfaches und übersichtliches Entwurfsmodell, durch das eine einfache Wartbarkeit (Trennung von View und Controller) und einfache Lesbarkeit gewährleistet wird.

**Datenbankabfragen abgesichert gegen SQL-Injections**
Bei SQL-Befehlen, die von Benutzereingaben abhängen, werden die Benutzereingaben über "bindValue" mit "Prepared-Statements" verknüpft.
Dadurch sind Benutzereingaben nur Vergleichswerte und nie Teil des SQL-Befehls.

**QSqlQueryModel und QSqlQuery abfragen statt QSqlTableModel**
QSqlQuery lässt reine SQL-Abfragen zu. Die Änderungen sollen im LogicController ablaufen (eigene Validierung, Fehlerbehandlung und Verlaufserstellung).
QSqlQueryModel ist read-only, so kann der Nutzer keine Daten in der View (am LogicController vorbei) ändern.

**Verlaufstabellen werden vollständig separat abgespeichert**  
Statt nur einen Zeitpunkt und eine Artikelnummer-Referenz zu speichern und in der Ansicht über SQL-JOIN zu verknüpfen, werden alle Attribute in den Verlaufstabellen gespeichert.
So wird sichergestellt, dass auch bei Artikeländerungen die Daten konsistent bleiben.
