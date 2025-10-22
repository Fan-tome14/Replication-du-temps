# Replication du temps

Ce projet a été réalisé en groupe dans le cadre du **Travail Pratique 2 (TP2)** du cours **Programmation réseau multi-joueurs en ligne**, offert à l’**Université du Québec à Chicoutimi (UQAC)**.  

---

- **Titre du projet :** Replication du temps
- **Date de réalisation :** 23 octobre 2025  
- **Type de projet :** Travail individuel  
- **Moteur de développement :** Unreal Engine

---

## Objectif du projet  
C’est une mise en œuvre de la **réplication du temps** dans un environnement multijoueur sous Unreal Engine, inspirée d’un algorithme de synchronisation proche du **NTP**. Le projet met en place un **compte à rebours synchronisé entre les joueurs**, déclenchant automatiquement le passage du **lobby au niveau de jeu**, puis le retour après une durée définie, le tout avec une **gestion du temps cohérente en réseau** et une **échappatoire via un menu pause**.


---

## Structure du projet  
- `Code/Source/` : Contient le code C++ du projet  
- `Code/Content/` : Contient les éléments multimédias (Blueprints, textures, sons, etc.)  
- `Code/Config/` : Contient les fichiers de configuration du projet
- `Code/myproject.uproject` : Permet de lancer le jeux sur Unreal Engine
- `README.md` : Présentation et documentation du projet  

---

## Remarque  
Ce projet a été développé exclusivement dans un but académique et ne constitue pas un produit fini. Il s’agit d’une **preuve de concept** visant à mettre en application les principes la programmation de réseau multi-joueurs en ligne dans un contexte vidéoludique.  
