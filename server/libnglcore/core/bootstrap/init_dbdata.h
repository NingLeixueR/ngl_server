// File overview: Declares helpers that seed demo and test data into bootstrap database tables.
#pragma once

#include "actor/generated/pb/db.pb.h"

void init_db_acc(const char* aname, int abeg);

void init_db_acc();

// Seed demo role records.
void init_db_role(const char* aname, int abeg);

void init_db_role();

// Seed demo bag records.
void init_db_bag(const char* aname, int abeg);

void init_db_bag();

void init_db_task(const char* aname, int abeg);

void init_db_task();

// Seed demo mailbox records.
void init_db_mail(int abeg);

void init_db_mail();

void init_db_rkv(int abeg);

void init_db_rkv();

void init_db_note();

void init_db_kv();

void init_db_fam();

void init_db_rank();

void seed_frd(pbdb::db_friends& afriends, int abeg, int aend, int aidx);

void init_db_frd();

void init_db_tlua();
