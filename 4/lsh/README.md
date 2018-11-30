# LSH - lightshell (chyba)

Prosty shell napisany w C zgodnie ze specyfikacją podaną przez prowadzącego.

### Użycie

Lsh przyjmuje tylko dokładną składnię komend, z kilkoma zasadami:
1. Pipe'y muszą być oddzielone spacjami.
2. Przekierowania do plików i stderr także, ponadto nie mogą zawierać spacji przed ścieżkami.
3. Uruchomienie procesu w tle można ustawić przez & na końcu po spacji po ostatniej komendzie.

Przykładowo:
```cat <lsh | hexdump -C | wc -l >a```
policzy wczyta z pliku `./lsh` i zapisze wynik w `./a` (utworzy go jeżeli trzeba i doda linię, jeżeli już istnnieje). Natomiast
```ls plik_ktory/nie/istnieje 2>/dev/null | wc -l```
wypisze `0 0 0`, ale ls nie wypisze błędu. `>` może być użyty w ostatniej komendzie w ciągu, `<` w pierwszej, `2>` w dowolnej.