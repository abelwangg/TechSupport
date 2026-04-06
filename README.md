# Tech Support Diagnosis Tool

This repo contains my implementation of the ECE 312H Tech Support Diagnosis Tool (Lab 4).

It's an interactive, self-learning binary decision tree written entirely in C. The program acts as a troubleshooting assistant that starts with a basic y/n question. If it suggests a fix that doesn't solve the user's problem, the program enters a "learning phase" where it asks for the correct solution and a distinguishing question, dynamically grafting new nodes into its knowledge base.

## ⚠️⚠️ Academic Integrity Warning LMAOO
If you are a student currently taking 312 at UT Austin, please **DON'T** copy or reference this code for your own lab assignment. This repository is public for archival purposes of my 20+ hrs of work only.

## Note on the Knowledge Base (`techsupport.dat`)
The actual learned decision tree is saved locally as a binary data file named `techsupport.dat`. Because this file updates constantly during gameplay and consists of unreadable binary bytes, it is intentionally excluded from version control. ggs don't copy.