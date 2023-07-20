L'archive ChallengeFolder.zip contient différents executable Linux utile pour le challenge.
Le premier executable 'ChallengeSetupScript' doit être lancé une fois les fichiers .json des graphes déplacés dans le même répertoire.
Cela va lancer automatiquement le recuit simulé à utiliser pour le challenge sur les différents fichiers json.
Chaque graphe va avoir son propre dossier de créé. A l'intérieur se trouve le dossier originalGraphe qui est le fichier d'origine qui ne sera jamais modifié.
Le dossier resultats contient les fichiers qui présentent une amélioration par rapport au fichier de départ.
Ces fichiers ne sont écris qu'après un minimum de 5 minutes d'exécution du recuit simulé pour éviter d'en écrire trop.

Différents exécutables sont présents dans chaque répertoire de fichiers.
RecuitSimuleChallenge est celui qui est lancé automatiquement.
RecuitSimuleAleatoire est le même exécutable mais partant d'une solution de départ aléatoire.
BestDeplacementChallenge effectue uniquement la partie post-traitement après le recuit en effectuant une descente simple en testant toutes les possibilités.
DebugGraphe fait différentes vérifications sur le graphe pour regarder si sa structure est correcte. (Par exemple deux emplacements au même endroit, boucle d'un noeud sur lui même ect...)

Ces 4 exécutables peuvent être lancés avec l'option -p pour choisir un fichier différents de celui qui est dans le répertoire originalGraphe.
L'option -s pour déterminer une seed fixe ainsi que l'option -c pour choisir le coeur d'exécution sont disponibles pour les trois premières heuristiques.
En raison d'un bug avec OGDF, il est recommandé de toujours utiliser l'option -c pour l'exécution RecuitSimuleChallenge.


OpenGLGraphe permet d'afficher le graphe dans une interface graphique ainsi que d'effectuer divers fonctions à l'aide de raccourcis claviers.
Il n'a pas d'option mais le premier argument peut être un chemin vers un fichier json.

Quelques raccourcis utiles:
I affiche/cache les intersections en les mettant en couleur.
& effectue un placement aléatoire.
Arrow-Left active le mode déplacement de noeud, le noeud sélectionné est affiché en bleu.
Clic gauche permet de sélectionner un noeud si le mode déplacement est activé.
Clic droit permet de sélectionner un emplacement. (occupé ou non)
S permet d'échanger la position du noeud sélectionné avec l'emplacement sélectionné.
F11 effectue un placement avec le Fast Multipole Multilevel Embedder.
' affiche le nombre courant d'intersection.
F10 double le nombre d'emplacement courant. (généré aléatoirement dans la zone)
F2 effectue un recuit simulé à basse température.
" effectue un recuit simulé classique.
