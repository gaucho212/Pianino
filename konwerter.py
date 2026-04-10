import mido
import sys

def midi_to_text(midi_file, output_file):
    try:
        mid = mido.MidiFile(midi_file)
    except Exception as e:
        print(f"Blad ladowania pliku MIDI: {e}")
        return

    # Mido odczytuje czas jako opóźnienia (delta time). Zamieniamy to na absolutny czas w sekundach.
    active_notes = {}
    finished_notes = []
    current_time = 0.0
    
    base_notes = ['C', 'Cs', 'D', 'Ds', 'E', 'F', 'Fs', 'G', 'Gs', 'A', 'As', 'B']
    
    for msg in mid:
        current_time += msg.time
        
        if not msg.is_meta:
            # Wciśnięcie klawisza
            if msg.type == 'note_on' and msg.velocity > 0:
                active_notes[msg.note] = current_time
            # Puszczenie klawisza (czasami zapisywane jako note_on z głośnością 0)
            elif msg.type == 'note_off' or (msg.type == 'note_on' and msg.velocity == 0):
                if msg.note in active_notes:
                    start_time = active_notes[msg.note]
                    duration = current_time - start_time
                    
                    # Konwersja numeru MIDI na Twój format (np. 60 -> C4, 61 -> C4s)
                    octave = (msg.note // 12) - 1
                    idx = msg.note % 12
                    
                    if "s" in base_notes[idx]:
                        note_name = f"{base_notes[idx][0]}{octave}s" # Np. C4s
                    else:
                        note_name = f"{base_notes[idx]}{octave}"     # Np. C4
                    
                    finished_notes.append((start_time, note_name, duration))
                    del active_notes[msg.note]
    
    # Sortujemy nuty chronologicznie
    finished_notes.sort(key=lambda x: x[0])
    
    with open(output_file, 'w') as f:
        for note in finished_notes:
            # Format docelowy: CzasStartu NazwaNuty CzasTrwania
            f.write(f"{note[0]:.2f} {note[1]} {note[2]:.2f}\n")
            
    print(f"Skonwertowano {len(finished_notes)} nut!")

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("Uzycie: python3 konwerter.py <plik_wejsciowy.mid> <plik_wyjsciowy.txt>")
    else:
        midi_to_text(sys.argv[1], sys.argv[2])