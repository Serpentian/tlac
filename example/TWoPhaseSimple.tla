----------------------------- MODULE TwoPhaseSimple ----------------------------

CONSTANT RM

VARIABLES rmState, tmState, tmPrepared, msgs
vars == <<rmState, tmState, tmPrepared, msgs>>

TypeOK ==
  /\ rmState \in [RM -> {"working", "prepared", "committed", "aborted"}]
  /\ tmState \in {"init", "done"}
  /\ tmPrepared \subseteq RM
  /\ msgs \subseteq Messages

Messages ==
  [type : {"Prepared"}, rm : RM] \cup [type : {"Commit", "Abort"}]

TPInit ==
  /\ rmState = [r \in RM |-> "working"]
  /\ tmState = "init"
  /\ tmPrepared = {}
  /\ msgs = {}

RMPrepare(r) ==
  /\ UNCHANGED <<tmState, tmPrepared>>
  /\ rmState[r] = "working"
  /\ rmState' = [rmState EXCEPT ![r] = "prepared"]
  /\ msgs' = msgs \cup {[type |-> "Prepared", rm |-> r]}

RMRcvCommitMsg(r) ==
  /\ UNCHANGED <<tmState, tmPrepared, msgs>>
  /\ [type |-> "Commit"] \in msgs
  /\ rmState' = [rmState EXCEPT ![r] = "committed"]

RMRcvAbortMsg(r) ==
  /\ UNCHANGED <<tmState, tmPrepared, msgs>>
  /\ [type |-> "Abort"] \in msgs
  /\ rmState' = [rmState EXCEPT ![r] = "aborted"]

TMRcvPrepared(r) ==
  /\ UNCHANGED <<rmState, tmState, msgs>>
  /\ tmPrepared' = tmPrepared \cup {r}
  /\ tmState = "init"
  /\ [type |-> "Prepared", rm |-> r] \in msgs

TMCommit ==
  /\ UNCHANGED <<rmState, tmPrepared>>
  /\ tmState = "init"
  /\ tmPrepared = RM
  /\ tmState' = "done"
  /\ msgs' = msgs \cup {[type |-> "Commit"]}

TMAbort ==
  /\ UNCHANGED <<rmState, tmPrepared>>
  /\ tmState = "init"
  /\ tmState' = "done"
  /\ msgs' = msgs \cup {[type |-> "Abort"]}

TPNext ==
  \/ TMCommit \/ TMAbort
  \/ \E r \in RM :
       \/ RMPrepare(r) \/ TMRcvPrepared(r)
       \/ RMRcvCommitMsg(r) \/ RMRcvAbortMsg(r)

Spec ==
  TPInit /\ [][TPNext]_vars

Consistent ==
  \A r1, r2 \in RM :
    ~(rmState[r1] = "aborted" /\ rmState[r2] = "committed")

THEOREM Spec => [](TypeOK /\ Consistent)

================================================================================
