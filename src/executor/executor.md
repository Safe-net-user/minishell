# Carte mentale — Réécriture de l'executor

## 0. Principes directeurs à fixer AVANT de coder

- [ ] **Qui possède la mémoire ?** Décider clairement : `t_tk->value` (heap) appartient à qui, `redir->file` appartient à qui, à quel moment est-il légitime de `free()` puis réécrire un pointeur. Écrire cette règle en commentaire en haut du fichier executor.
- [ ] **Où est libéré l'AST ?** Un seul point de sortie normal (`main` après `executor()`), et un chemin explicite pour les sorties anormales (`exit` builtin, erreurs fatales).
- [ ] **Convention fork vs no-fork** : quels builtins tournent dans le process parent (cd, export, unset, exit — doivent modifier l'état du shell) vs lesquels *pourraient* tourner en enfant (echo, pwd, env — pas obligatoire mais à trancher une fois pour toutes).

---

## 1. Dispatch des types de noeuds AST

- [ ] `NODE_CMD` — commande simple
- [ ] `NODE_PIPE` — pipeline `|`
- [ ] `NODE_AND` — `&&` (exécute droite seulement si gauche == 0)
- [ ] `NODE_OR` — `||` (exécute droite seulement si gauche != 0)
- [ ] Noeud vide / `head == NULL` → status 0, pas de crash
- [ ] Propagation correcte de `mms->last_status` à chaque étage (pas juste au niveau racine)

## 2. Commande simple (`NODE_CMD`)

- [ ] Expansion des tokens (`expand()`) — une seule fois, au bon moment
- [ ] Cas `tokens` vide après expansion (ex: `"$EMPTY"` seul) → doit quand même traiter les redirections (`execute_redir_only`) et ne rien exécuter
- [ ] Détection builtin vs externe

### 2bis. Expansion — architecture et cas à ne pas oublier

**À trancher en premier : est-ce que le sujet exige word-splitting et/ou globbing ?** Si non, documenter ce choix explicitement (commentaire + éventuellement un mot dans le README) pour ne pas se poser la question 50 fois pendant la review.

- [ ] `$VAR` → valeur de l'env, ou chaîne vide si absente (jamais crash)
- [ ] `$?` → dernier exit status, cas spécial à coder en dur (pas dans la table d'env)
- [ ] `$` seul (pas suivi d'un identifiant valide) → reste littéralement `$`, pas d'expansion
- [ ] `$0`, `$1`... (positional params) : normalement hors scope minishell (pas d'args au shell), à vérifier
- [ ] Quote removal : après expansion, les guillemets `'` et `"` eux-mêmes doivent disparaître du résultat final (ils ne servent qu'à contrôler l'expansion, ils ne doivent jamais apparaître dans l'argv exécuté)
- [ ] `'$VAR'` (simple quotes) → PAS expansé, littéral
- [ ] `"$VAR"` (double quotes) → expansé, mais le résultat reste **un seul token** même s'il contient des espaces (pas de split)
- [ ] `$VAR` non quoté → expansé, ET (si implémenté) splitté sur IFS si le résultat contient des espaces/tabs/newlines
- [ ] Si word-splitting implémenté : un token qui explose en plusieurs mots doit quand même se comporter comme des tokens `TOK_WORD` normaux ensuite (flags, type) — pas de perte d'info
- [ ] Si globbing implémenté (`*`, `?`, `[...]`) : uniquement si non quoté ; aucun match trouvé → le pattern reste tel quel littéralement (comportement bash par défaut sans `nullglob`)
- [ ] **`redir->file` : le résultat de l'expansion (avec split/glob appliqués comme pour un argument normal) doit obligatoirement se réduire à UN SEUL mot.** Si l'expansion produit plusieurs mots → erreur `bash: FICHIER: ambiguous redirect`, exit status non-zéro (souvent 1), la commande ne s'exécute pas (mais les autres redirections déjà faites avant dans l'ordre restent appliquées — comportement bash à vérifier précisément si tu veux le coller à 100%)
- [ ] Le nom du délimiteur d'un heredoc (`<< EOF`) n'est lui-même **jamais expansé** — seul son état "quoté ou non" détermine si le corps du heredoc sera expansé
- [ ] Ordre d'expansion vs redirections : dans une commande genre `echo $A > $B`, `$A` et `$B` sont deux expansions indépendantes, chacune avec sa propre règle (split pour `$A`, single-word-or-error pour `$B`)
- [ ] Résolution de `PATH` :
  - [ ] commande avec `/` → chemin direct, pas de lookup PATH
  - [ ] commande sans `/` → cherche dans chaque dossier de `$PATH`
  - [ ] `$PATH` absent de l'environnement → comportement bash (souvent : seulement `.` ou rien, à vérifier vs bash réel)
  - [ ] commande introuvable → exit code **127** + message `command not found`
  - [ ] commande trouvée mais pas exécutable / permission refusée → exit code **126**
  - [ ] commande qui est un dossier → **126** + `Is a directory`
- [ ] `execve` échoue pour une autre raison → gérer `errno` proprement, toujours `exit()` dans l'enfant (jamais `return` — évite le double-fork zombie)

## 3. Builtins — liste et où ils s'exécutent

Builtins qui **modifient l'état du shell** → toujours dans le process **parent**, jamais forké :
- [ ] `cd` (change `cwd`, doit mettre à jour `PWD`/`OLDPWD` dans l'env)
- [ ] `export` (modifie la table d'env)
- [ ] `unset` (modifie la table d'env)
- [ ] `exit` (doit terminer le shell lui-même)

Builtins qui peuvent tourner indifféremment (mais convention à fixer) :
- [ ] `echo` (gérer `-n`, gérer `-nnn` multiple, PAS d'autres options)
- [ ] `pwd` (doit gérer le cas où `getcwd()` échoue, ex: dossier supprimé)
- [ ] `env` (affiche l'environnement, sans argument seulement selon le sujet)

Cas spécial :
- [ ] Builtin appelé **au milieu d'un pipeline** (`ls | cd /tmp | wc`) → doit quand même s'exécuter dans un fork dans ce cas précis pour ne pas casser le pipe, MAIS alors `cd` n'aura aucun effet sur le shell parent (comportement bash normal). Vérifier que ton executor gère ce cas différemment du builtin en position isolée.

### 3bis. Détails par builtin à ne pas oublier

- [ ] `cd` : sans argument → `$HOME` (erreur propre si `$HOME` absent de l'env) ; `cd -` → `$OLDPWD` (erreur si absent) et **affiche le nouveau chemin** comme bash le fait ; met à jour `PWD` et `OLDPWD` dans la table d'env à chaque appel réussi, pas juste `chdir()` seul ; chemin relatif vs absolu
- [ ] `export` sans argument → affiche l'env trié (souvent format `declare -x KEY="value"`, à vérifier selon ton sujet)
- [ ] `export KEY` (sans `=`) → ajoute la variable comme exportée mais sans valeur (distinct de `KEY=""`)
- [ ] `export KEY=` (avec `=` mais valeur vide) → valeur vide, pas absente
- [ ] `export 1INVALID=x` (identifiant invalide, commence par un chiffre) → erreur `not a valid identifier`, exit 1, mais ne doit pas empêcher le traitement des autres arguments valides sur la même ligne
- [ ] `unset` sur une variable inexistante → pas d'erreur, silencieux (comportement bash)
- [ ] `unset PATH` → doit vraiment retirer `PATH`, casse volontairement la résolution des commandes externes ensuite (à tester, comportement attendu)
- [ ] `env` avec arguments → erreur ou comportement spécifique selon le sujet (souvent hors scope, `env` sans arg uniquement)

## 4. Redirections

- [ ] `<` — input simple
- [ ] `>` — output truncate
- [ ] `>>` — output append
- [ ] `<<` heredoc :
  - [ ] expansion des variables dans le heredoc SAUF si délimiteur quoté (`<< "EOF"` ou `<< 'EOF'`)
  - [ ] `Ctrl+C` pendant un heredoc → doit interrompre proprement, ne pas exécuter la commande, exit status 130
  - [ ] `Ctrl+D` (EOF) avant le délimiteur → warning bash-like (`warning: here-document delimited by end-of-file`), continue quand même
  - [ ] plusieurs heredocs sur la même commande → le dernier "gagne" (écrase les fd précédents), mais TOUS doivent être lus/consommés dans l'ordre
- [ ] Plusieurs redirections du même type (`> a > b`) → seule la dernière compte, mais tous les fichiers doivent être créés/tronqués (effet de bord bash réel)
- [ ] Ordre d'application : les redirections s'appliquent dans l'ordre d'écriture, pas juste "la dernière l'emporte" sans effet de bord
- [ ] Erreur d'ouverture de fichier (permission, dossier inexistant) → message d'erreur propre + exit status non-zéro, ne doit pas crasher tout le pipeline
- [ ] Sauvegarde/restauration des fd std (in/out) pour les builtins non-forkés — actuellement fait via `dup`/`dup2`, à garder mais vérifier fuite de fd si une redirection échoue en cours de route (fd déjà dupé pas fermé)
- [ ] `redir->file` expansé en plusieurs mots → erreur `ambiguous redirect` (voir section 2bis), PAS un crash, PAS une prise silencieuse du premier mot
- [ ] Chaque heredoc successif (`<<1 <<2 <<3`) doit utiliser/fermer son propre fd sans écraser celui du précédent avant que tous soient traités — bug déjà repéré dans l'ancienne version, à surveiller spécifiquement dans les tests de la nouvelle

## 5. Pipelines (`|`)

- [ ] Création des pipes (`pipe()`) AVANT les forks
- [ ] Fermeture de TOUS les fd inutiles dans chaque process (parent ET chaque enfant) — cause n°1 de deadlock/hang si oublié
- [ ] `wait()` sur tous les enfants du pipeline, pas juste le dernier — sinon zombies
- [ ] Exit status du pipeline = celui du **dernier** process (comportement bash standard, `$?`)
- [ ] `SIGPIPE` : un `write` sur un pipe fermé (ex: `yes | head -1`) ne doit pas faire planter minishell
- [ ] Builtin en tête, milieu, ou fin de pipeline — chaque position doit fonctionner
- [ ] Pipeline avec une seule commande mal formée au milieu → les autres doivent quand même se lancer (ou pas — à décider selon le sujet, souvent bash lance quand même le reste)

## 6. Gestion des signaux

- [ ] `SIGINT` (Ctrl+C) en mode interactif (attente de `readline`) → nouvelle ligne, réaffiche prompt, n'interrompt pas le shell
- [ ] `SIGINT` pendant l'exécution d'une commande externe → doit tuer l'enfant, pas le parent, remonter exit code 130
- [ ] `SIGINT` pendant un heredoc → interrompt la saisie du heredoc spécifiquement (déjà vu : `interrupted` dans `heredoc_gnl`)
- [ ] `SIGQUIT` (Ctrl+\) → ignoré par le shell en interactif, mais comportement par défaut (core dump message) pour les enfants
- [ ] **Le bug qu'on vient de croiser** : boucle infinie / spam `^C` + `Interrupted system call` après plusieurs `Ctrl+C` → à isoler et fixer avant de considérer ce chapitre fini
- [ ] Restauration des handlers par défaut dans les processus enfants AVANT `execve` (déjà fait dans `execute_child`, à garder)
- [ ] Pas de variable globale non-sig-atomic modifiée dans un handler (uniquement `sig_atomic_t volatile`, norme 42 stricte sur ce point : une seule variable globale autorisée)
- [ ] État terminal de `readline` après une interruption : `rl_on_new_line()` + `rl_replace_line("", 0)` + `rl_redisplay()` dans le handler `SIGINT` interactif, sinon le prompt peut se retrouver décalé/dupliqué après plusieurs `Ctrl+C` (symptôme observé : spam `^C` en boucle)
- [ ] Vérifier qu'aucun appel bloquant (`read`, `waitpid`) mal protégé contre `EINTR` ne fait boucler le programme au lieu de gérer l'interruption proprement — le message `Interrupted system call` observé est un symptôme direct de ça
- [ ] Un seul jeu de handlers "interactif" + fonctions symétriques de bascule par contexte (voir design discuté : `set_signals_exec_parent`, `set_signals_child`, `set_signals_heredoc`, `set_signals_interactif`) — **règle d'or : toute fonction qui bascule les signaux doit restaurer l'interactif avant de retourner, sur TOUS ses chemins de sortie, y compris les erreurs**

## 7. Gestion mémoire / cleanup (déjà largement traité mais à revalider)

- [ ] Un seul point de free pour l'AST par itération de boucle (`free_ast_values`)
- [ ] `exit` builtin doit libérer l'AST courant avant `exit()` — fix qu'on vient de poser, à conserver dans la réécriture
- [ ] Stack allocator (`sa`) : bien comprendre ce qu'il libère (structures) vs ce qu'il NE libère PAS (les `value` en heap) — documenter ça clairement dans le nouveau code pour éviter de refaire l'erreur
- [ ] `cmd_path` caché en hashtable — vérifier qu'il est bien libéré à la fin du programme, pas par commande
- [ ] Aucun leak sur les chemins d'erreur (fork échoué, pipe échoué, open échoué) — souvent oublié parce que testé uniquement sur le cas nominal

## 8. Codes de sortie — tableau de référence à respecter

| Cas | Code |
|---|---|
| Succès | 0 |
| Erreur générique / builtin erreur usage | 1 ou 2 selon builtin |
| Commande trouvée, pas exécutable | 126 |
| Commande introuvable | 127 |
| Terminé par un signal | 128 + numéro du signal |
| `exit` avec argument non numérique | 255 (+ message erreur bash) |
| `exit` avec trop d'arguments | 1, **et n'exit PAS le shell** (comportement bash réel à vérifier) |

## 9. Cas limites à tester une fois l'executor réécrit

- [ ] `""` (ligne vide) → ne fait rien, pas de segfault
- [ ] `   ` (que des espaces) → idem
- [ ] `| ls` ou `ls |` (pipe mal placé) → erreur de syntaxe propre, pas de crash
- [ ] `ls > ` (redirection sans nom de fichier) → erreur de syntaxe
- [ ] Commande avec beaucoup d'arguments / tokens (vérifier que `count_tokens`/allocation ne déborde jamais)
- [ ] Variable d'env inexistante (`$NOPE`) → chaîne vide, pas crash
- [ ] `$?` après chaque type de commande (builtin, externe, pipeline, signal)
- [ ] Enchaînement `cd` relatif puis absolu, `cd -`, `cd` sans argument (vers `$HOME`)
- [ ] Deux `exit` d'affilée dans un pipeline ou après `&&`
- [ ] `echo $X > $Y` où `$Y` contient un espace → `ambiguous redirect`, pas de crash
- [ ] `cat << EOF` puis `Ctrl+C` immédiat → shell reste utilisable après, prompt propre
- [ ] Une commande qui n'écrit rien mais dont le pipe amont ferme tôt (`yes | true`) → pas de `SIGPIPE` qui tue minishell
- [ ] Redirection combinée avec pipeline (`cat < in.txt | grep x > out.txt`) → chaque bout gère sa propre redirection sans casser le pipe des autres

## 10. Pièges transverses identifiés pendant cette session (à garder en tête)

- [ ] **Écrasement de pointeur sans free préalable** : `ptr = nouvelle_valeur` sur un pointeur heap déjà alloué → toujours vérifier qu'il n'y a pas un ancien `malloc` à libérer juste avant. C'est la cause exacte de deux des trois bugs corrigés aujourd'hui.
- [ ] **Process qui meurt sans repasser par le cleanup normal** : tout chemin qui appelle `exit()` directement (pas juste le builtin `exit`, aussi les erreurs fatales dans un enfant après fork) doit soit ne rien devoir libérer (cas normal pour un enfant forké, qui a sa propre copie mémoire de toute façon — pas un problème), soit explicitement libérer ce qui doit l'être si c'est le process **parent/shell principal** qui sort par ce chemin.
- [ ] **Confusion entre mémoire de l'arène (`stack_alloc`) et mémoire heap (`malloc`/`ft_strdup`)** : documenter noir sur blanc, dans le header ou en commentaire de tête de fichier, quelles structs vivent dans l'arène (reset global, jamais free individuellement) vs quels champs à l'intérieur restent des pointeurs heap classiques (à free individuellement). C'est ce mélange qui rend les leaks difficiles à racommoder après coup.

## Comment s'en servir

Coche au fur et à mesure que tu réécris/valides chaque point. Pour chaque nouvelle fonction que tu écris, demande-toi : *"est-ce que je référence une struct/pointeur venant du lexer, du parser, ou d'un `stack_alloc` — et si oui, qui a la responsabilité de le libérer ?"* — c'est cette question précise qui a causé tous les leaks qu'on a corrigés aujourd'hui.






test 136-140, 144
test 53, 54
test 171, 172

exit rerheu doit pas exit et doit return 2
apres ctrlC echo $? doit afficher 130
'' '' '' et ''"" doit afficher minishell: : command not found
ctrlC dans heredoc : pb affichage
<< limiter | cmd doit d'abord ouvrir un heredoc et ensuite executer cmd quand le limiter ferme le heredoc
  (alors que ton minishell fait l'inverse + le heredoc deconne)


valgrind --track-fds=yes --leak-check=full --show-leak-kinds=all --suppressions=readline.supp ./minishell
