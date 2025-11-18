from bs4 import BeautifulSoup
import re

INPUT_FILE = "/Users/andrewaballantyne/Downloads/c++/Poster/Dickinson College Commentaries.html"
OUTPUT_FILE = "corpus.txt"

def extract_entries():
    # Load HTML
    with open(INPUT_FILE, "r", encoding="utf-8") as f:
        soup = BeautifulSoup(f.read(), "html.parser")

    # All headword cells are in <td class="views-field views-field-field-display-headwords">
    headword_cells = soup.find_all("td", class_="views-field views-field-field-display-headwords")

    results = []

    for cell in headword_cells:
        text = cell.get_text(strip=True)

        # Skip empty or non-lexical items
        if not text:
            continue

        # ---- VERBS ----
        # Identify infinitives (2nd principal part)
        infinitives = re.findall(r"\b[a-zA-Z]*?([āē]re|ere|īre)\b", text)
        if infinitives:
            # The full infinitive string matches what ends in those patterns
            full_inf = re.findall(r"\b[a-zA-Zāēīōū]+?(?:āre|ēre|ere|īre)\b", text)
            results.extend(full_inf)
            continue  # Skip the noun processing if verb found

        # ---- NOUNS ----
        # Extract first principal part (nominative)
        # Rule: take the string up to the first space or hyphen
        first_pp = re.split(r"[ \-]", text)[0]

        # Filter out particles (ac, ad, etc.)
        if len(first_pp) > 2:
            results.append(first_pp)

    return results


def save_corpus(items):
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        for item in items:
            f.write(item + "\n")


def main():
    items = extract_entries()
    save_corpus(items)
    print(f"Saved {len(items)} items to {OUTPUT_FILE}")


if __name__ == "__main__":
    main()
