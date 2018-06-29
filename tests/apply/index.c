#include "clar_libgit2.h"
#include "apply_helpers.h"

static git_repository *repo;

#define TEST_REPO_PATH "merge-recursive"

void test_apply_index__initialize(void)
{
	git_oid oid;
	git_commit *commit;

	repo = cl_git_sandbox_init(TEST_REPO_PATH);

	git_oid_fromstr(&oid, "539bd011c4822c560c1d17cab095006b7a10f707");
	cl_git_pass(git_commit_lookup(&commit, repo, &oid));
	cl_git_pass(git_reset(repo, (git_object *)commit, GIT_RESET_HARD, NULL));
	git_commit_free(commit);
}

void test_apply_index__cleanup(void)
{
	cl_git_sandbox_cleanup();
}

void test_apply_index__generate_diff(void)
{
	git_oid a_oid, b_oid;
	git_commit *a_commit, *b_commit;
	git_tree *a_tree, *b_tree;
	git_diff *diff;
	git_diff_options diff_opts = GIT_DIFF_OPTIONS_INIT;
	git_apply_options opts = GIT_APPLY_OPTIONS_INIT;

	struct merge_index_entry index_expected[] = {
		{ 0100644, "ffb36e513f5fdf8a6ba850a20142676a2ac4807d", 0, "asparagus.txt" },
		{ 0100644, "68f6182f4c85d39e1309d97c7e456156dc9c0096", 0, "beef.txt" },
		{ 0100644, "4b7c5650008b2e747fe1809eeb5a1dde0e80850a", 0, "bouilli.txt" },
		{ 0100644, "c4e6cca3ec6ae0148ed231f97257df8c311e015f", 0, "gravy.txt" },
		{ 0100644, "68af1fc7407fd9addf1701a87eb1c95c7494c598", 0, "oyster.txt" },
		{ 0100644, "a7b066537e6be7109abfe4ff97b675d4e077da20", 0, "veal.txt" },
	};
	size_t index_expected_cnt = sizeof(index_expected) /
		sizeof(struct merge_index_entry);

	git_oid_fromstr(&a_oid, "539bd011c4822c560c1d17cab095006b7a10f707");
	git_oid_fromstr(&b_oid, "7c7bf85e978f1d18c0566f702d2cb7766b9c8d4f");
	cl_git_pass(git_commit_lookup(&a_commit, repo, &a_oid));
	cl_git_pass(git_commit_lookup(&b_commit, repo, &b_oid));

	cl_git_pass(git_commit_tree(&a_tree, a_commit));
	cl_git_pass(git_commit_tree(&b_tree, b_commit));

	cl_git_pass(git_diff_tree_to_tree(&diff, repo, a_tree, b_tree, &diff_opts));

	opts.location = GIT_APPLY_LOCATION_INDEX;
	cl_git_pass(git_apply(repo, diff, &opts));

	validate_apply_index(repo, index_expected, index_expected_cnt);
	validate_workdir_unchanged(repo);

	git_diff_free(diff);
	git_tree_free(a_tree);
	git_tree_free(b_tree);
	git_commit_free(a_commit);
	git_commit_free(b_commit);
}

void test_apply_index__parsed_diff(void)
{
	git_diff *diff;
	git_apply_options opts = GIT_APPLY_OPTIONS_INIT;

	struct merge_index_entry index_expected[] = {
		{ 0100644, "ffb36e513f5fdf8a6ba850a20142676a2ac4807d", 0, "asparagus.txt" },
		{ 0100644, "68f6182f4c85d39e1309d97c7e456156dc9c0096", 0, "beef.txt" },
		{ 0100644, "4b7c5650008b2e747fe1809eeb5a1dde0e80850a", 0, "bouilli.txt" },
		{ 0100644, "c4e6cca3ec6ae0148ed231f97257df8c311e015f", 0, "gravy.txt" },
		{ 0100644, "68af1fc7407fd9addf1701a87eb1c95c7494c598", 0, "oyster.txt" },
		{ 0100644, "a7b066537e6be7109abfe4ff97b675d4e077da20", 0, "veal.txt" },
	};
	size_t index_expected_cnt = sizeof(index_expected) /
		sizeof(struct merge_index_entry);

	cl_git_pass(git_diff_from_buffer(&diff,
		DIFF_MODIFY_TWO_FILES, strlen(DIFF_MODIFY_TWO_FILES)));

	opts.location = GIT_APPLY_LOCATION_INDEX;
	cl_git_pass(git_apply(repo, diff, &opts));

	validate_apply_index(repo, index_expected, index_expected_cnt);
	validate_workdir_unchanged(repo);

	git_diff_free(diff);
}

void test_apply_index__removes_file(void)
{
	git_diff *diff;
	git_apply_options opts = GIT_APPLY_OPTIONS_INIT;

	struct merge_index_entry index_expected[] = {
		{ 0100644, "f51658077d85f2264fa179b4d0848268cb3475c3", 0, "asparagus.txt" },
		{ 0100644, "68f6182f4c85d39e1309d97c7e456156dc9c0096", 0, "beef.txt" },
		{ 0100644, "4b7c5650008b2e747fe1809eeb5a1dde0e80850a", 0, "bouilli.txt" },
		{ 0100644, "68af1fc7407fd9addf1701a87eb1c95c7494c598", 0, "oyster.txt" },
		{ 0100644, "94d2c01087f48213bd157222d54edfefd77c9bba", 0, "veal.txt" },
	};
	size_t index_expected_cnt = sizeof(index_expected) /
	    sizeof(struct merge_index_entry);

	cl_git_pass(git_diff_from_buffer(&diff, DIFF_DELETE_FILE,
		strlen(DIFF_DELETE_FILE)));

	opts.location = GIT_APPLY_LOCATION_INDEX;
	cl_git_pass(git_apply(repo, diff, &opts));

	validate_apply_index(repo, index_expected, index_expected_cnt);
	validate_workdir_unchanged(repo);

	git_diff_free(diff);
}

void test_apply_index__adds_file(void)
{
	git_diff *diff;
	git_apply_options opts = GIT_APPLY_OPTIONS_INIT;

	struct merge_index_entry index_expected[] = {
		{ 0100644, "f51658077d85f2264fa179b4d0848268cb3475c3", 0, "asparagus.txt" },
		{ 0100644, "68f6182f4c85d39e1309d97c7e456156dc9c0096", 0, "beef.txt" },
		{ 0100644, "4b7c5650008b2e747fe1809eeb5a1dde0e80850a", 0, "bouilli.txt" },
		{ 0100644, "c4e6cca3ec6ae0148ed231f97257df8c311e015f", 0, "gravy.txt" },
		{ 0100644, "6370543fcfedb3e6516ec53b06158f3687dc1447", 0, "newfile.txt" },
		{ 0100644, "68af1fc7407fd9addf1701a87eb1c95c7494c598", 0, "oyster.txt" },
		{ 0100644, "94d2c01087f48213bd157222d54edfefd77c9bba", 0, "veal.txt" },
	};
	size_t index_expected_cnt = sizeof(index_expected) /
	    sizeof(struct merge_index_entry);

	cl_git_pass(git_diff_from_buffer(&diff,
		DIFF_ADD_FILE, strlen(DIFF_ADD_FILE)));

	opts.location = GIT_APPLY_LOCATION_INDEX;
	cl_git_pass(git_apply(repo, diff, &opts));

	validate_apply_index(repo, index_expected, index_expected_cnt);
	validate_workdir_unchanged(repo);

	git_diff_free(diff);
}

void test_apply_index__modified_workdir_with_unmodified_index_is_ok(void)
{
	git_diff *diff;
	git_apply_options opts = GIT_APPLY_OPTIONS_INIT;

	const char *diff_file = DIFF_MODIFY_TWO_FILES;

	struct merge_index_entry index_expected[] = {
		{ 0100644, "ffb36e513f5fdf8a6ba850a20142676a2ac4807d", 0, "asparagus.txt" },
		{ 0100644, "68f6182f4c85d39e1309d97c7e456156dc9c0096", 0, "beef.txt" },
		{ 0100644, "4b7c5650008b2e747fe1809eeb5a1dde0e80850a", 0, "bouilli.txt" },
		{ 0100644, "c4e6cca3ec6ae0148ed231f97257df8c311e015f", 0, "gravy.txt" },
		{ 0100644, "68af1fc7407fd9addf1701a87eb1c95c7494c598", 0, "oyster.txt" },
		{ 0100644, "a7b066537e6be7109abfe4ff97b675d4e077da20", 0, "veal.txt" },
	};
	size_t index_expected_cnt = sizeof(index_expected) /
		sizeof(struct merge_index_entry);

	struct merge_index_entry workdir_expected[] = {
		{ 0100644, "68f6182f4c85d39e1309d97c7e456156dc9c0096", 0, "beef.txt" },
		{ 0100644, "4b7c5650008b2e747fe1809eeb5a1dde0e80850a", 0, "bouilli.txt" },
		{ 0100644, "c4e6cca3ec6ae0148ed231f97257df8c311e015f", 0, "gravy.txt" },
		{ 0100644, "68af1fc7407fd9addf1701a87eb1c95c7494c598", 0, "oyster.txt" },
		{ 0100644, "f75ba05f340c51065cbea2e1fdbfe5fe13144c97", 0, "veal.txt" }
	};
	size_t workdir_expected_cnt = sizeof(workdir_expected) /
		sizeof(struct merge_index_entry);

	/* mutate the workdir and leave the index matching HEAD */
	cl_git_rmfile("merge-recursive/asparagus.txt");
	cl_git_rewritefile("merge-recursive/veal.txt", "Hello, world.\n");

	cl_git_pass(git_diff_from_buffer(&diff, diff_file, strlen(diff_file)));

	opts.location = GIT_APPLY_LOCATION_INDEX;
	cl_git_pass(git_apply(repo, diff, &opts));

	validate_apply_index(repo, index_expected, index_expected_cnt);
	validate_apply_workdir(repo, workdir_expected, workdir_expected_cnt);

	git_diff_free(diff);
}