import mido
import sys

def midi_to_text(midi_file, output_file):
    try:
        mid = mido.MidiFile(midi_file)
    except Exception as e:
        print(f"Blad ladowania pliku MIDI: {e}")
        return

    # Przechowuje aktualnie wciśnięte nuty i ich czas startu
    active_notes = {}
    finished_notes = []
    
    # Mido podaje czas jako opóźnienia, więc sumujemy je, by uzyskać rzeczywisty czas w sekundach
    current_time = 0.0
    
    base_notes = ['C', 'Cs', 'D', 'Ds', 'E', 'F', 'Fs', 'G', 'Gs', 'A', 'As', 'B']
    
    for msg in mid:
        current_time += msg.time
        
        if not msg.is_meta:
            if msg.type == 'note_on' and msg.velocity > 0:
                active_notes[msg.note] = current_time
                
            elif msg.type == 'note_off' or (msg.type == 'note_on' and msg.velocity == 0):
                if msg.note in active_notes:
                    start_time = active_notes[msg.note]
                    duration = current_time - start_time
                    
                    # Zamiana numeru MIDI (0-127) na standardową notację (np. 60 -> C4)
                    octave = (msg.note // 12) - 1
                    idx = msg.note % 12
                    
                    # Dopasowanie nazw dla półtonów pod nazewnictwo siatek w naszym modelu 3D
                    if "s" in base_notes[idx]:
                        note_name = f"{base_notes[idx][0]}{octave}s"
                    else:
                        note_name = f"{base_notes[idx]}{octave}"
                    
                    finished_notes.append((start_time, note_name, duration))
                    del active_notes[msg.note]
    
    # Sortujemy nuty chronologicznie, żeby ułatwić odczyt w głównym programie C++
    finished_notes.sort(key=lambda x: x[0])
    
    with open(output_file, 'w') as f:
        for note in finished_notes:
            f.write(f"{note[0]:.2f} {note[1]} {note[2]:.2f}\n")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Uzycie: python konwerter.py <plik_midi> <plik_wyjsciowy>")
    else:
        midi_to_text(sys.argv[1], sys.argv[2])