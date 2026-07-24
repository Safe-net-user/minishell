#!/usr/bin/env bash
#
# norm_format.sh - Corrige automatiquement les points MECANIQUES de la Norme 42
# (indentation en tabs, espaces en trop, lignes vides sales, fin de fichier)
# et signale (sans corriger) les points structurels qui doivent etre traites
# a la main (fonctions trop longues, mots-cles interdits, etc).
#
# Usage:
#   ./norm_format.sh fichier1.c fichier2.c ...
#   ./norm_format.sh --dir src/            # traite tous les .c/.h sous src/
#   ./norm_format.sh --no-backup fichier.c # ne garde pas de .bak
#
# Par defaut, un fichier.c.bak est cree avant modification.

set -euo pipefail

BACKUP=1
FILES=()
DIR=""

while [[ $# -gt 0 ]]; do
	case "$1" in
		--no-backup)
			BACKUP=0
			shift
			;;
		--dir)
			DIR="$2"
			shift 2
			;;
		*)
			FILES+=("$1")
			shift
			;;
	esac
done

if [[ -n "$DIR" ]]; then
	while IFS= read -r -d '' f; do
		FILES+=("$f")
	done < <(find "$DIR" -type f \( -name '*.c' -o -name '*.h' \) -print0)
fi

if [[ ${#FILES[@]} -eq 0 ]]; then
	echo "Usage: $0 [--no-backup] [--dir <path>] fichier1.c [fichier2.c ...]" >&2
	exit 1
fi

PYFIX=$(cat <<'PYEOF'
import sys

def fix_file(path):
	with open(path, "r", encoding="utf-8", errors="surrogateescape") as f:
		text = f.read()

	# Normalise les fins de ligne (CRLF -> LF)
	text = text.replace("\r\n", "\n").replace("\r", "\n")

	lines = text.split("\n")
	out_lines = []

	in_block_comment = False

	for raw_line in lines:
		line = raw_line

		# --- 1. Separer l'indentation de tete du reste de la ligne ---
		i = 0
		while i < len(line) and line[i] in " \t":
			i += 1
		leading = line[:i]
		rest = line[i:]

		# --- 2. Convertir l'indentation de tete en tabs (unite tab = 4) ---
		col = 0
		for ch in leading:
			if ch == "\t":
				col += 4 - (col % 4)
			else:
				col += 1
		n_tabs = col // 4
		remainder = col % 4
		new_leading = "\t" * n_tabs + (" " * remainder if remainder else "")

		# --- 3. Ligne vide (apres tete) -> vraiment vide ---
		if rest.strip("") == "" and rest.strip() == "":
			out_lines.append("")
			continue

		# --- 4. Collapse des doubles espaces DANS le code, hors strings/commentaires ---
		result_chars = []
		i = 0
		n = len(rest)
		space_run = 0
		while i < n:
			ch = rest[i]
			nxt = rest[i + 1] if i + 1 < n else ""

			if in_block_comment:
				result_chars.append(ch)
				if ch == "*" and nxt == "/":
					result_chars.append(nxt)
					i += 2
					in_block_comment = False
					continue
				i += 1
				continue

			if ch == "/" and nxt == "*":
				result_chars.append(ch)
				result_chars.append(nxt)
				i += 2
				in_block_comment = True
				continue

			if ch == "/" and nxt == "/":
				# commentaire single-line : on ne touche plus rien jusqu'a la fin
				result_chars.append(rest[i:])
				i = n
				continue

			if ch == '"' or ch == "'":
				quote = ch
				result_chars.append(ch)
				i += 1
				while i < n:
					c2 = rest[i]
					result_chars.append(c2)
					if c2 == "\\" and i + 1 < n:
						result_chars.append(rest[i + 1])
						i += 2
						continue
					i += 1
					if c2 == quote:
						break
				continue

			if ch == " ":
				space_run += 1
				if space_run == 1:
					result_chars.append(ch)
				i += 1
				continue
			else:
				space_run = 0
				result_chars.append(ch)
				i += 1

		rest = "".join(result_chars)

		# --- 5. Trailing whitespace ---
		rest = rest.rstrip(" \t")

		if rest == "":
			out_lines.append("")
		else:
			out_lines.append(new_leading + rest)

	# --- 6. Fin de fichier : une seule newline finale, pas de lignes vides en trop ---
	while out_lines and out_lines[-1] == "":
		out_lines.pop()
	out_lines.append("")  # une seule ligne vide finale (= \n final)

	new_text = "\n".join(out_lines)

	with open(path, "w", encoding="utf-8", errors="surrogateescape") as f:
		f.write(new_text)


def analyze_file(path):
	warnings = []
	with open(path, "r", encoding="utf-8", errors="surrogateescape") as f:
		lines = f.readlines()

	forbidden = ["for", "switch", "goto", "do"]
	brace_depth_at_func_start = None
	func_start_line = None
	func_count = 0
	in_func = False
	depth = 0

	for idx, line in enumerate(lines, start=1):
		stripped = line.rstrip("\n")

		if len(stripped.expandtabs(4)) > 80:
			warnings.append(f"  L{idx}: ligne > 80 colonnes")

		for kw in forbidden:
			import re
			if re.search(rf"(^|[^a-zA-Z0-9_]){kw}\s*[\(\.]", stripped) or re.search(rf"(^|[^a-zA-Z0-9_]){kw}\s*$", stripped):
				if re.search(rf"(^|[^a-zA-Z0-9_]){kw}([^a-zA-Z0-9_]|$)", stripped):
					warnings.append(f"  L{idx}: mot-cle interdit possible '{kw}'")

		if re.search(r"[^:]\?[^:]*:", stripped) and "?" in stripped and "//" not in stripped.split("?")[0]:
			pass  # ternaire: trop de faux positifs (ex: pointeurs, macros) -> pas fiable, on ne le signale pas ici

		# Comptage grossier de profondeur d'accolades hors strings/commentaires (heuristique simple)
		depth += stripped.count("{") - stripped.count("}")

		# Detection tres grossiere de debut de fonction : ligne finissant par ')' suivie de '{' seul sur la ligne suivante
		if stripped == "{" and not in_func and depth == 1:
			in_func = True
			func_start_line = idx
			func_count += 1
		elif stripped == "}" and in_func and depth == 0:
			length = idx - func_start_line - 1
			if length > 25:
				warnings.append(f"  L{func_start_line}-{idx}: fonction de {length} lignes (> 25)")
			in_func = False

	if func_count > 5:
		warnings.append(f"  Total: {func_count} fonctions dans ce fichier (> 5, verifie si tout est du meme fichier .c)")

	return warnings


for path in sys.argv[1:]:
	fix_file(path)
	warnings = analyze_file(path)
	if warnings:
		print(f"\n[MANUEL] {path} - a verifier a la main :")
		for w in warnings:
			print(w)
PYEOF
)

for f in "${FILES[@]}"; do
	if [[ ! -f "$f" ]]; then
		echo "Fichier introuvable, ignore : $f" >&2
		continue
	fi
	if [[ "$BACKUP" -eq 1 ]]; then
		cp "$f" "$f.bak"
	fi
done

python3 -c "$PYFIX" "${FILES[@]}"

echo ""
echo "Termine. $([[ $BACKUP -eq 1 ]] && echo "Backups en .bak a cote de chaque fichier." || echo "Pas de backup (--no-backup).")"
echo ""
echo "NON corrige automatiquement (a faire a la main) :"
echo "  - Fonctions > 25 lignes, > 5 fonctions/fichier, > 4 parametres"
echo "  - for / do-while / switch / case / goto / operateur ternaire (interdits)"
echo "  - Regroupement des declarations en debut de bloc + ligne vide apres"
echo "  - Etoiles de pointeur collees au nom (char *foo, pas char* foo)"
echo "  - Header 42 (non verifie par la norminette elle-meme, cf doc II.1-II.10)"
echo "  - VLA, types implicites, assignation multiple"
echo ""
echo "Pour une verification officielle et exhaustive, installe la vraie norminette :"
echo "  pip install norminette --break-system-packages && norminette <fichiers>"