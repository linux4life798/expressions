/**
 * @file workspace.c
 *
 * @date Apr 25, 2014
 * @author Craig Hesling
 */
#include <string.h> // strncpy() , strlen()
#include "errors.h"
#include "workspace.h"

static struct workspace {
	char name[WORKSPACE_NAME_SIZE]; ///< NULL terminated string that identifies this data. Guaranteed to be NULL terminated.
	void *data;
} WS[WORKSPACE_SIZE];

#define WS_IS_UNSET(x) (WS[x].name[0] == '\0')
#define WS_UNSET(x)    WS[x].name[0] = '\0'

/*
 * Optimization
 * This optimization simply places an uper limit on the number of possible set entries.
 * Note: As entries are unset from the front of WS, this indicator will become less helpful.
 */
/// Indicates upper limit of set workspace entries
static int ws_last = -1;
#define WS_LAST          ws_last
#define WS_LAST_RESET    ws_last = -1
#define WS_LAST_SET(x)   ws_last = ((x) > ws_last) ?(x):ws_last
#define WS_LAST_UNSET(x) ws_last = ((x) == ws_last)?(ws_last-1):ws_last

void
workspace_init(void) {
	int i;
	for(i=0; i<WORKSPACE_SIZE; i++) {
		WS_UNSET(i);
	}
	WS_LAST_RESET;
}

#define WS_NOTFOUND WORKSPACE_SIZE

/**
 * Find the index of the workspace entry with a matching name.
 * @param name The name to match.
 * @return The first associated workspace entry index or WS_NOTFOUND if not found.
 */
static int
workspace_find_name(char *name) {
	int windex; // workspace-index
	assert(name);
	assert(name[0] != '\0'); // name cannot be the UNSET indicator

	// search each workspace element until found or not found.
	for (windex=0; windex < WORKSPACE_SIZE; windex++) {
		int cindex; // char-index

		// optimization - stop when the remaining are unset
		if (windex > WS_LAST) {
			windex = WORKSPACE_SIZE; // just get out of here (technically, windex would become WORKSPACE_SIZE+1 by the end)
		}

		// name cannot match an unset workspace entry
		if(WS_IS_UNSET(windex)) {
			continue;
		}

		// compare names char by char.
		for (cindex=0; cindex<WORKSPACE_NAME_SIZE; cindex++) {
			// if non-matching chars
			if(name[cindex] != WS[windex].name[cindex]) {
				// not match
				break; // next workspace entry
			}
			// if end of matching
			if(name[cindex] == '\0') {
				// found match
				assert((0 <= windex) && (windex < WORKSPACE_SIZE));
				return windex;
			}
		}
	}
	return WS_NOTFOUND;
}

/**
 * Find the index of the workspace entry with a matching name.
 * @param start The index to start searching from. Usually 0.
 * @return The first unset workspace entry or WS_NOTFOUND if not found.
 */
static int
workspace_find_unset(int start) {
	int windex; // workspace-index
	assert(start >= 0);

	// search each workspace element until found or not found.
	for (windex=start; windex < WORKSPACE_SIZE; windex++) {
		// if workspace entry is unset
		if(WS_IS_UNSET(windex)) {
			assert((0 <= windex) && (windex < WORKSPACE_SIZE));
			return windex;
		}
	}
	return WS_NOTFOUND;
}

int
workspace_set(char *name, void *data) {
	int windex;
	assert(name);

	// name must cannot be the UNSET indicator
	if(name[0] == '\0')
		return WORKSPACE_NAME;
	// name must be no greater than WORKSPACE_NAME_SIZE-1 (for NULL byte)
	if(strlen(name) >= WORKSPACE_NAME_SIZE)
		return WORKSPACE_NAME;

	// find previous entry for name
	windex = workspace_find_name(name);

	// if no previous entries, use a new entry
	if(windex == WS_NOTFOUND) {
		// find first unset entry
		windex = workspace_find_unset(0);
		// if could not find unset entry, error out
		if (windex == WS_NOTFOUND) {
			return WORKSPACE_FULL;
		}
	}

	assert((0 <= windex) && (windex < WORKSPACE_NAME_SIZE));
	// should have legitimate windex now
	strncpy(WS[windex].name, name, WORKSPACE_NAME_SIZE); // safe to copy because length was verified above
	WS[windex].data = data;
	WS_LAST_SET(windex);

	return WORKSPACE_OK;
}

void
workspace_unset(char *name) {
	int windex;
	windex = workspace_find_name(name);
	if(windex != WS_NOTFOUND) {
		WS_UNSET(windex);
		WS_LAST_UNSET(windex); // Optimization
	}
}

void *
workspace_get(char *name) {
	int windex;
	assert(name);
	///@todo Create a return value for bad name like @ref workspace_set with WORKSPACE_NAME
	assert(name[0] != '\0'); // name cannot be the UNSET indicator

	// search for name
	windex = workspace_find_name(name);

	// if found
	if(windex != WS_NOTFOUND) {
		return WS[windex].data;
	}
	// if not found
	return WORKSPACE_NOTSET;
}

#ifdef WORKSPACE_TEST_MAIN
/*
 * Some simple tests for the workspace class.
 *
 * gcc -g -DDEBUG -DWORKSPACE_TEST_MAIN -o ws workspace.c
 * or
 * gcc -g -DDEBUG -DWORKSPACE_TEST_MAIN -o ws errors.c workspace.c
 */

// Want to make sure NDEBUG is not defined when assert.h is included for assert statements
#include <stdio.h>

#define INT_TO_PTR(x) ((void *)(long int)(x))
#define PTR_TO_INT(x) ((int)(long int)(x))

int main() {
	int i;
	int ret;
	workspace_init();

	// set all entries - all ok
	for (i=0; i<WORKSPACE_SIZE; i++) {
		char name[WORKSPACE_NAME_SIZE];
		sprintf(name, "var%d", i);
		printf("Setting: %s to %d\n", name, (i+25));
		ret = workspace_set(name, INT_TO_PTR(i+25));
		assert(ret == WORKSPACE_OK);
	}

	// get all entries - all ok
	for (i=WORKSPACE_SIZE-1; i>=0; i--) {
		void *value;
		char name[WORKSPACE_NAME_SIZE];
		sprintf(name, "var%d", i);
		printf("Getting: %s\n", name);
		value = workspace_get(name);
		printf("got: %d\n", PTR_TO_INT(value));
		assert(value != WORKSPACE_NOTSET);
		assert(value == INT_TO_PTR(i+25));
	}

	// try extra entry set - fail
	puts("");
	printf("Setting extra: varnew to 125\n");
	ret = workspace_set("varnew", INT_TO_PTR(125));
	printf("ret: %s\n", (ret==WORKSPACE_OK)?"WORKSPACE_OK":((ret == WORKSPACE_FULL))?"WORKSPACE_FULL":"WORKSPACE_NAME");
	assert(ret == WORKSPACE_FULL);

	puts("Unset var5");
	workspace_unset("var5");

	puts("Try setting extra again");
	// try extra entry set - ok
	puts("");
	printf("Setting extra: varnew to 125\n");
	ret = workspace_set("varnew", INT_TO_PTR(125));
	printf("ret: %s\n", (ret==WORKSPACE_OK)?"WORKSPACE_OK":((ret == WORKSPACE_FULL))?"WORKSPACE_FULL":"WORKSPACE_NAME");
	assert(ret == WORKSPACE_OK);

	puts("");
	puts("Remaining Tests:");

	puts("try previous named entry - ok");
	// try previous named entry - ok
	ret = workspace_set("var1", INT_TO_PTR(125));
	assert(ret == WORKSPACE_OK);

	puts("reset workspace for another go - ok");
	// reset workspace for another go - ok
	workspace_init();

	puts("add simple one - ok");
	// add simple one - ok
	ret = workspace_set("var1", INT_TO_PTR(125));
	assert(ret == WORKSPACE_OK);


	puts("some stuff");
	// some stuff
	ret = workspace_set("var2", INT_TO_PTR(1000007));
	assert(ret == WORKSPACE_OK);
	ret = workspace_set("var2", INT_TO_PTR(1000007));
	assert(ret == WORKSPACE_OK);
	return 0;
}
#endif // #ifdef WORKSPACE_TEST_MAIN

/* vim: set ts=4 sw=4 expandtab: */
